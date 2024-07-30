/*-------------------------------------------------------------------------
  cpu2.v - multi-cycle f8 core

  Copyright (c) 2024, Philipp Klaus Krause philipp@colecovision.eu)

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2, or (at your option) any
  later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
-------------------------------------------------------------------------*/

`begin_keywords "1800-2009"

`include "opcode.v"
`include "alu2.v"

//`define F8L // Enable for simplified f8l core.

typedef enum logic [2:0]
{
	FLAG_H,	// Half-carry
	FLAG_C,	// Carry
	FLAG_N,	// Negative
	FLAG_Z,	// Zero
	FLAG_O,	// Overflow
	FLAG_5,
	FLAG_6,
	FLAG_7
} flagname_t;

// Could be written nicer using unpacked structs, but icarus does not yet support those.
module registers(output logic [15:0] x, y, z, input logic [1:0] addr_in, input logic [15:0] data_in, input logic [1:0] write_en,
	output logic [7:0] f, input logic [7:0] next_f,
	output logic [15:0] sp, input logic [15:0] next_sp,
	output logic [15:0] pc, input logic [15:0] next_pc,
	output logic [23:0] oldinst, input logic [23:0] inst,
	output logic oldinst_valid, input next_oldinst_valid,
	output logic oldinst_loadsop, input next_oldinst_loadsop,
	output logic interrupt_active, input next_interrupt_active,
	input logic clk, reset);
	logic [15:0] gpregs[3];

	assign x = gpregs[0];
	assign y = gpregs[1];
	assign z = gpregs[2];

	always_ff @(posedge clk)
	begin
		if (write_en[0])
			gpregs[addr_in][7:0] <= data_in[7:0];
		if (write_en[1])
			gpregs[addr_in][15:8] <= data_in[15:8];
		if (reset)
			f <= {3'b000, next_f[4:0]};
		else
			f <= next_f;
		pc <= next_pc;
		sp <= next_sp;
		if (reset)
			oldinst <= OPCODE_NOP;
		else
			oldinst <= inst;
		if (reset)
			oldinst_valid <= 0;
		else
			oldinst_valid <= next_oldinst_valid;
		if (reset)
			oldinst_loadsop <= 0;
		else
			oldinst_loadsop <= next_oldinst_loadsop;
		if (reset)
			interrupt_active <= 0;
		else
			interrupt_active <= next_interrupt_active;
	end
endmodule

function automatic logic opcode_loads_upper(opcode_t opcode);
	return(opcode_is_dir(opcode) || opcode_is_zrel(opcode));
endfunction

function automatic logic opcode_loads_operand(opcode_t opcode);
	return(opcode_is_8_immd(opcode) || opcode_is_16_immd(opcode) || opcode_is_dir_read(opcode) || opcode_is_sprel_read(opcode) || opcode_is_zrel_read(opcode) || opcode == OPCODE_MAD_X_IZ_YL ||
	opcode == OPCODE_CALL_IMMD || opcode == OPCODE_LD_XL_IY ||  opcode == OPCODE_XCH_XL_IY || opcode == OPCODE_CAX_IY_ZL_XL || opcode == OPCODE_POP_XL || opcode == OPCODE_MSK_IY_XL_IMMD || opcode == OPCODE_RET || opcode == OPCODE_RETI || opcode == OPCODE_POPW_Y || opcode == OPCODE_JP_IMMD || opcode == OPCODE_LDW_Y_IY || opcode == OPCODE_LDW_Y_D || opcode == OPCODE_ADDW_SP_D || opcode == OPCODE_ADDW_Y_D || opcode == OPCODE_XCHW_X_IY || opcode == OPCODE_CAXW_IY_Z_X || opcode == OPCODE_LDI_IZ_IY || opcode == OPCODE_LDWI_IZ_IY
`ifndef F8L
	|| opcode == OPCODE_LD_XL_YREL || opcode == OPCODE_LDW_Y_YREL
`endif
	);
endfunction

typedef enum logic [2:0] {
	ACCSEL_XL_Y = 3'b000,
	ACCSEL_YL_Z = 3'b010,
	ACCSEL_ZL_X = 3'b011,
	ACCSEL_SWAPOP = 3'b100,
	ACCSEL_XH_Y = 3'b101,
	ACCSEL_YH_Z = 3'b110,
	ACCSEL_ZH_Y = 3'b111
} accsel_t;

module cpu
	(output logic [14:0] mem_read_addr_even, input logic [7:0] mem_read_data_even, output logic [14:0] mem_write_addr_even, output logic [7:0] mem_write_data_even, output logic mem_write_en_even,
	output logic [14:0] mem_read_addr_odd, input logic [7:0] mem_read_data_odd, output logic [14:0] mem_write_addr_odd, output logic [7:0] mem_write_data_odd, output logic mem_write_en_odd,
	input logic clk, reset, interrupt, output logic trap);

	logic [15:0] x, y, z;
	logic [1:0] regwrite_en;
	logic [1:0] regwrite_addr;
	logic [15:0] regwrite_data;
	logic [7:0] f, next_f;
	logic [15:0] sp, next_sp;
	logic [15:0] pc, next_pc;
	logic [23:0] inst, oldinst;
	logic oldinst_valid, next_oldinst_valid;
	logic oldinst_loadsop, next_oldinst_loadsop;
	logic interrupt_active, next_interrupt_active, interrupt_start;

	logic [15:0] mem_read_addr;
	logic [15:0] mem_read_data;
	logic [15:0] memwrite_data;
	logic [15:0] memwrite_addr;
	logic [1:0] memwrite_en;
	logic [7:0] opcode;
	logic [15:0] memop;
	logic [15:0] memop_addr;
	accsel_t accsel;
	logic [15:0] acc16;

	registers registers(.addr_in(regwrite_addr), .data_in(regwrite_data), .write_en(regwrite_en), .*);

	assign opcode = inst[7:0];
	assign accsel = accsel_t'(f[7:5]);
	assign acc16 = (accsel == ACCSEL_YL_Z) ? z :
			(accsel == ACCSEL_ZL_X) ? x :
			y;

	logic loading_upper_inst, loading_operand, inst_upper_valid, execute;

	// Memory read
	always_comb
	begin
		if(opcode_is_8_immd(opcode) && opcode != OPCODE_MSK_IY_XL_IMMD || opcode_is_16_immd(opcode) || opcode == OPCODE_CALL_IMMD || opcode == OPCODE_JP_IMMD || opcode == OPCODE_LDW_Y_D || opcode == OPCODE_ADDW_SP_D || opcode == OPCODE_ADDW_Y_D)
			memop_addr = pc + 1;
		else if(opcode_is_dir_read(opcode))
			memop_addr = inst[23:8];
		else if(opcode_is_sprel_read(opcode))
			memop_addr = sp + {8'h00, inst[15:8]};
		else if(opcode_is_zrel_read(opcode))
			memop_addr = z + inst[23:8];
		else if(opcode == OPCODE_LD_XL_IY || opcode == OPCODE_XCH_XL_IY || opcode == OPCODE_CAX_IY_ZL_XL || opcode == OPCODE_MSK_IY_XL_IMMD || opcode == OPCODE_LDW_Y_IY || opcode == OPCODE_XCHW_X_IY || opcode == OPCODE_CAXW_IY_Z_X ||
			opcode == OPCODE_LDI_IZ_IY || opcode == OPCODE_LDWI_IZ_IY)
			memop_addr = acc16;
`ifndef F8L
		else if(opcode_is_8_1_yrel(opcode) || opcode == OPCODE_LD_XL_YREL || opcode == OPCODE_LDW_Y_YREL)
			memop_addr = y + {8'h00, inst[15:8]};
`endif
		else if(opcode == OPCODE_POP_XL || opcode == OPCODE_RET || opcode == OPCODE_RETI || opcode == OPCODE_POPW_Y)
			memop_addr = sp;
		else
			memop_addr = 'x;
		if(loading_upper_inst)
		begin
			mem_read_addr = pc + 1;
		end
		else if (loading_operand)
		begin
			mem_read_addr = memop_addr;
		end
		else
		begin
			mem_read_addr = next_pc;
		end
		if(mem_read_addr[0])
		begin
			mem_read_addr_odd = mem_read_addr[15:1];
			mem_read_addr_even = mem_read_addr[15:1] + 1;
		end
		else
		begin
			mem_read_addr_even = mem_read_addr[15:1];
			mem_read_addr_odd = mem_read_addr[15:1];
		end
	end

	// Memory write
	always_comb
	begin
		mem_write_addr_even = memwrite_addr[0] ? memwrite_addr[15:1] + 1 : memwrite_addr[15:1];
		mem_write_addr_odd = memwrite_addr[15:1];
		mem_write_data_even = memwrite_addr[0] ? memwrite_data[15:8] : memwrite_data[7:0];
		mem_write_data_odd = memwrite_addr[0] ? memwrite_data[7:0] : memwrite_data[15:8];
		mem_write_en_even = memwrite_addr[0] ? memwrite_en[1] : memwrite_en[0];
		mem_write_en_odd = memwrite_addr[0] ? memwrite_en[0] : memwrite_en[1];
	end

	// Interrupts
	assign interrupt_start = interrupt && !interrupt_active;
	assign next_interrupt_active = interrupt_start && !interrupt_active || interrupt_active && opcode != OPCODE_RETI;

	// Instruction handling
	assign loading_upper_inst = opcode_loads_upper (opcode) && !oldinst_valid && !execute;
	assign inst_upper_valid = oldinst_valid;
	assign next_oldinst_loadsop = opcode_loads_operand(opcode) && !loading_upper_inst && !execute;
	assign execute = !reset && interrupt_start || (!opcode_loads_upper(opcode) || inst_upper_valid || oldinst_loadsop) && (!opcode_loads_operand(opcode) || oldinst_loadsop);
	assign next_oldinst_valid = loading_upper_inst;
	assign loading_operand = opcode_loads_operand(opcode) && !execute;
	
	always_comb
	begin
		if(reset)
			inst = OPCODE_NOP;
		else if(oldinst_valid)
			inst = {pc[0] ? mem_read_data_odd : mem_read_data_even, oldinst[15:0]};
		else if(oldinst_loadsop)
			inst = oldinst;
		else
			inst = pc[0] ? {8'hx, mem_read_data_even, mem_read_data_odd} : {8'hx, mem_read_data_odd , mem_read_data_even};
		if(opcode_loads_operand(opcode) && execute)
			memop = memop_addr[0] ? {mem_read_data_even, mem_read_data_odd} : {mem_read_data_odd , mem_read_data_even};
		else
			memop = 'x;
	end

	// Program counter
	always_comb
	begin
		accsel_t accsel;
		logic [1:0] acc16_addr;
		logic [15:0] next_pc_noint;
		accsel = accsel_t'(f[7:5]);
		if(!execute || trap)
			next_pc_noint = pc;
		else if(opcode == OPCODE_CALL_IMMD || opcode == OPCODE_JP_IMMD)
			next_pc_noint = memop;
		else if(opcode == OPCODE_CALL_Y || opcode == OPCODE_JP_Y)
			next_pc_noint = acc16;
		else if(opcode == OPCODE_RET || opcode == OPCODE_RETI)
			next_pc_noint = memop;
		else if(opcode == OPCODE_JR_D ||
			opcode == OPCODE_JRC_D && f[FLAG_C] || opcode == OPCODE_JRNC_D && !f[FLAG_C] ||
			opcode == OPCODE_JRN_D && f[FLAG_N] || opcode == OPCODE_JRNN_D && !f[FLAG_N] ||
			opcode == OPCODE_JRZ_D && f[FLAG_Z] || opcode == OPCODE_JRNZ_D && !f[FLAG_Z] ||
			opcode == OPCODE_JRNO_D && ((accsel == ACCSEL_SWAPOP) ^ !f[FLAG_O]) ||
			opcode == OPCODE_JRSGE_D && !(f[FLAG_N] ^ f[FLAG_O]) || opcode == OPCODE_JRSLT_D && (f[FLAG_N] ^ f[FLAG_O]) ||
			opcode == OPCODE_JRSLE_D && ((accsel == ACCSEL_SWAPOP) ^ (f[FLAG_Z] || (f[FLAG_N] ^ f[FLAG_O]))) ||
			opcode == OPCODE_JRLE_D && ((accsel == ACCSEL_SWAPOP) ^ (!f[FLAG_C] || f[FLAG_Z]))
`ifndef F8L
			|| opcode == OPCODE_DNJNZ_YH_D && ((accsel == ACCSEL_ZL_X) ? x[15:8] : (accsel == ACCSEL_YL_Z) ? z[15:8] : y[15:8]) != 8'h01
`endif
			)
			next_pc_noint = signed'(pc) + signed'(inst[15:8]);
		else
			next_pc_noint = pc + opcode_instsize(opcode);
		if(reset)
			next_pc = 16'h4000;
		else if(interrupt_start)
			next_pc = 16'h4004;
		else
			next_pc = next_pc_noint;
	end

	// Instruction execution
	assign trap = !reset && (opcode == OPCODE_TRAP);
	always_comb
	begin
		
		logic swapop;
		logic [1:0] acc8_addr, op8_addr, acc16_addr, op16_addr;
		logic [1:0] acc8_en, op8_en;
		logic [7:0] acc8, imm8, mem8, op8;
		logic [15:0] imm16, mem16, op16;

		// Temporaries, need to be here, rather than in blocks below, to work around yosys issue #4325.
		logic[7:0] result8;
		addsub_result_t addsub_result;
		logic[8:0] result9;
		logic[7:0] newacc8;
		logic[15:0] result16;
		result8 = 'x;
		addsub_result = 'x;
		result9 = 'x;
		newacc8= 'x;
		result16 = 'x;

		swapop = (accsel == ACCSEL_SWAPOP);

		acc8_addr = (accsel == ACCSEL_ZL_X || accsel == ACCSEL_ZH_Y) ? 2 : (accsel == ACCSEL_YL_Z || accsel == ACCSEL_YH_Z) ? 1 : 0;
		op8_addr = 'x;
		acc16_addr = (accsel == ACCSEL_ZL_X) ? 0 : (accsel == ACCSEL_YL_Z || accsel == ACCSEL_YH_Z) ? 2 : 1;
		op16_addr = 'x;
		acc8_en = (accsel == ACCSEL_XH_Y || accsel == ACCSEL_YH_Z || accsel == ACCSEL_ZH_Y) ? 2'b10 : 2'b01;
		op8_en = 'x;
		acc8 = (accsel == ACCSEL_XH_Y) ? x[15:8] :
			(accsel == ACCSEL_YL_Z) ? y[7:0] :
			(accsel == ACCSEL_YH_Z) ? y[15:8] :
			(accsel == ACCSEL_ZL_X) ? z[7:0] :
			(accsel == ACCSEL_ZH_Y) ? z[15:8] :
			x[7:0];
		imm8 = opcode == OPCODE_MSK_IY_XL_IMMD ? inst[15:8] : memop[7:0];
		mem8 = memop[7:0];
		op8 = 'x;
		imm16 = memop[15:0];
		mem16 = memop[15:0];
		op16 = 'x;
		regwrite_data = 'x;
		regwrite_addr = 'x;
		regwrite_en = 2'b00;
		next_f = {ACCSEL_XL_Y, f[4:0]};
		memwrite_data = 'x;
		memwrite_addr = 'x;
		memwrite_en = 2'b00;
		next_sp = sp;

		if(execute)
		begin
			if(opcode_is_8_immd(opcode))
				op8 = imm8;
			else if(opcode_is_8_2_mem(opcode) || opcode_is_8_1_mem(opcode) && !opcode_is_clr(opcode))
				op8 = mem8;
			else if(opcode_is_8_2_zl(opcode))
			begin
				op8_addr = 2;
				op8_en = 2'b01;
				op8 = z[7:0];
			end
			else if(opcode_is_8_2_xh(opcode))
			begin
				op8_addr = 0;
				op8_en = 2'b10;
				op8 = x[15:8];
			end
			else if(opcode_is_8_2_yl(opcode))
			begin
				op8_addr = 1;
				op8_en = 2'b01;
				op8 = y[7:0];
			end
			else if(opcode_is_8_2_yh(opcode))
			begin
				op8_addr = 1;
				op8_en = 2'b10;
				op8 = y[15:8];
			end
			else if(opcode_is_8_1_xl(opcode))
			begin
				op8_addr = acc8_addr;
				op8_en = acc8_en;
				op8 = acc8;
			end

			if(opcode_is_16_immd(opcode))
				op16 = imm16;
			else if(opcode_is_16_2_mem(opcode) || opcode_is_16_1_mem(opcode) && !opcode_is_clrw(opcode))
				op16 = mem16;
			else if(opcode_is_16_2_x(opcode) || opcode == OPCODE_XCHW_X_IY || opcode == OPCODE_LDW_Y_X || opcode == OPCODE_LDW_X_Y || opcode == OPCODE_LDW_IY_X || opcode == OPCODE_LDW_YREL_X)
			begin
				op16_addr = (accsel == ACCSEL_YL_Z || accsel == ACCSEL_YH_Z) ? 1 :
					(accsel == ACCSEL_ZL_X || accsel == ACCSEL_ZH_Y) ? 2 :
					0;
				op16 = (accsel == ACCSEL_YL_Z || accsel == ACCSEL_YH_Z) ? y :
					(accsel == ACCSEL_ZL_X || accsel == ACCSEL_ZH_Y) ? z :
					x;
			end
			else if(opcode_is_16_1_y(opcode)
`ifndef F8L
				|| opcode == OPCODE_DNJNZ_YH_D
`endif
				)
			begin
				op16_addr = acc16_addr;
				op16 = acc16;
			end

			if (opcode_is_8_2_mem(opcode) && !opcode_is_cp(opcode) || opcode_is_8_1_mem(opcode) && !opcode_is_tst(opcode) ||
				opcode_is_16_1_mem(opcode) && !opcode_is_tstw(opcode) || opcode_is_16_2_mem(opcode) && opcode != OPCODE_CPW_Y_IMMD ||
				opcode == OPCODE_LDW_SPREL_Y || opcode == OPCODE_LDW_DIR_Y || opcode == OPCODE_LDW_ZREL_Y ||
				opcode == OPCODE_XCH_F_SPREL || opcode == OPCODE_XCHW_Y_SPREL)
			begin
				if (opcode_is_dir(opcode))
					memwrite_addr = inst[23:8];
				else if (opcode_is_sprel(opcode))
					memwrite_addr = sp + {8'h00, inst[15:8]};
				else
					memwrite_addr = z + inst[23:8];
			end

			if(interrupt_start)
			begin
				if(opcode != OPCODE_RETI)
				begin
					memwrite_data = pc;
					memwrite_addr = sp - 2;
					memwrite_en = 2'b11;
					next_sp = sp - 2;
				end
				next_f = f;
			end
			else if(opcode == OPCODE_SWAPOP)
				next_f[7:5] = ACCSEL_SWAPOP;
			else if(opcode == OPCODE_ALTACC1)
				next_f[7:5] = ACCSEL_XH_Y;
			else if(opcode == OPCODE_ALTACC2)
				next_f[7:5] = ACCSEL_YL_Z;
			else if(opcode == OPCODE_ALTACC3)
				next_f[7:5] = ACCSEL_ZL_X;
			else if(opcode == OPCODE_ALTACC4)
				next_f[7:5] = ACCSEL_YH_Z;
			else if(opcode == OPCODE_ALTACC5)
				next_f[7:5] = ACCSEL_ZH_Y;
			else if(opcode_is_8_2(opcode) || opcode_is_8_1(opcode) && !opcode_is_push(opcode))
			begin
				//logic[7:0] result8;
				//result8 = 'x;
				if(opcode_is_sub(opcode) || opcode_is_cp(opcode))
				begin
					
					//addsub_result_t addsub_result;
`ifndef F8L
					if (swapop)
						addsub_result = addsub ({8'h00, ~acc8}, {8'h00, op8}, 1, 0);					
					else
`endif
						addsub_result = addsub ({8'h00, acc8}, {8'h00, ~op8}, 1, 0);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, addsub_result.h};
					result8 = addsub_result.result[7:0];
				end
				else if(opcode_is_sbc(opcode))
				begin
					//addsub_result_t addsub_result;
`ifndef F8L
					if (swapop)
						addsub_result = addsub ({8'h00, ~acc8}, {8'h00, op8}, f[FLAG_C], 0);
					else
`endif
						addsub_result = addsub ({8'h00, acc8}, {8'h00, ~op8}, f[FLAG_C], 0);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, addsub_result.h};
					result8 = addsub_result.result[7:0];
				end
				else if(opcode_is_add(opcode))
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub ({8'h00, acc8}, {8'h00, op8}, 0, 0);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, addsub_result.h};
					result8 = addsub_result.result[7:0];
				end
				else if(opcode_is_adc(opcode))
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub ({8'h00, acc8}, {8'h00, op8}, f[FLAG_C], 0);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, addsub_result.h};
					result8 = addsub_result.result[7:0];
				end
				else if(opcode_is_or(opcode))
				begin
					result8 = acc8 | op8;
					next_f = {3'b000, f[FLAG_O], !(|result8), result8[7], f[FLAG_C], f[FLAG_H]};
				end
				else if(opcode_is_and(opcode))
				begin
					result8 = acc8 & op8;
					next_f = {3'b000, f[FLAG_O], !(|result8), result8[7], f[FLAG_C], f[FLAG_H]};
				end
				else if(opcode_is_xor(opcode))
				begin
					result8 = acc8 ^ op8;
					next_f = {3'b000, f[FLAG_O], !(|result8), result8[7], f[FLAG_C], f[FLAG_H]};
				end
				else if(opcode_is_srl(opcode))
				begin
					//logic[8:0] result9;
					result9 = {1'b0, op8};
					result8 = result9[8:1];
					next_f = {3'b000, f[FLAG_O], !(|result8[7:0]), f[FLAG_N], result9[0], f[FLAG_H]};
				end
				else if(opcode_is_sll(opcode))
				begin
					//logic[8:0] result9;
					result9 = {op8, 1'b0};
					result8 = result9[7:0];
					next_f = {3'b000, f[FLAG_O], !(|result8[7:0]), f[FLAG_N], result9[8], f[FLAG_H]};
				end
				else if(opcode_is_rrc(opcode))
				begin
					//logic[8:0] result9;
					result9 = {f[FLAG_C], op8};
					result8 = result9[8:1];
					next_f = {3'b000, f[FLAG_O], !(|result8[7:0]), f[FLAG_N], result9[0], f[FLAG_H]};
				end
				else if(opcode_is_rlc(opcode))
				begin
					//logic[8:0] result9;
					result9 = {op8, f[FLAG_C]};
					result8 = result9[7:0];
					next_f = {3'b000, f[FLAG_O], !(|result8[7:0]), f[FLAG_N], result9[8], f[FLAG_H]};
				end
				else if(opcode_is_inc(opcode))
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub ({8'h00, op8}, 16'h0001, 0, 0);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, addsub_result.h};
					result8 = addsub_result.result[7:0];
				end
				else if(opcode_is_dec(opcode))
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub ({8'h00, op8}, 16'h00fe, 1, 0);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, addsub_result.h};
					result8 = addsub_result.result[7:0];
				end
				else if(opcode_is_clr(opcode))
				begin
					result8 = 8'h00;
				end
				else if(opcode_is_tst(opcode))
				begin
					next_f = {3'b000, ^op8, !(|op8), op8[7], 1'b0, f[FLAG_H]};
				end
				else if(opcode == OPCODE_SRA_XL)
				begin
					result8 = {op8[7], op8[7:1]};
					next_f[FLAG_Z] = !result8;
					next_f[FLAG_C] = op8[0];
				end
				else if(opcode == OPCODE_DA_XL)
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub ({8'h00, op8}, {8'h00, dadjust(op8, f[FLAG_C], f[FLAG_H])}, 0, 0);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, addsub_result.h};
					result8 = addsub_result.result[7:0];
				end
				else if(opcode == OPCODE_BOOL_XL)
				begin
					result8 = |op8;
					next_f[FLAG_Z] = !result8;
				end
				else if(opcode == OPCODE_THRD_XL)
				begin
					result8 = 0;
					next_f[FLAG_Z] = !result8;
				end

				if (!swapop && !opcode_is_cp(opcode) && !opcode_is_8_1_mem(opcode) && !opcode_is_tst(opcode))
				begin
					regwrite_data = {result8, result8};
					regwrite_addr = acc8_addr;
					regwrite_en = acc8_en;
				end
				else if (!opcode_is_cp(opcode) && !opcode_is_tst(opcode) && !opcode_is_8_2_immd(opcode))
				begin
					if(opcode_is_8_2_mem(opcode) || opcode_is_8_1_mem(opcode))
					begin
						memwrite_data = result8;
						memwrite_en = 2'b01;
					end
					else
					begin
						regwrite_data = {result8, result8};
						regwrite_addr = op8_addr;
						regwrite_en = op8_en;
					end
				end
			end
			else if(opcode == OPCODE_NOP)
			begin
				next_f = f;
			end
			else if(opcode == OPCODE_JP_IMMD)
			begin
				next_f = f;
			end
			else if(opcode == OPCODE_CALL_IMMD)
			begin
				memwrite_data = pc + 3;
				memwrite_addr = sp - 2;
				memwrite_en = 2'b11;
				next_sp = sp - 2;
			end
			else if(opcode == OPCODE_CALL_Y)
			begin
				memwrite_data = pc + 1;
				memwrite_addr = sp - 2;
				memwrite_en = 2'b11;
				next_sp = sp - 2;
			end
`ifndef F8L
			else if(opcode_is_xchb(opcode))
			begin
				//logic [7:0] result8, newacc8;
				newacc8 = 8'h00;
				result8 = mem8;
				if(opcode == OPCODE_XCHB_XL_MM_0)
				begin
					newacc8[0] = mem8[0];
					result8[0] = acc8[0];
				end
				else if (opcode == OPCODE_XCHB_XL_MM_1)
				begin
					newacc8[0] = mem8[1];
					result8[1] = acc8[0];
				end
				else if (opcode == OPCODE_XCHB_XL_MM_2)
				begin
					newacc8[0] = mem8[2];
					result8[2] = acc8[0];
				end
				else if (opcode == OPCODE_XCHB_XL_MM_3)
				begin
					newacc8[0] = mem8[3];
					result8[3] = acc8[0];
				end
				else if (opcode == OPCODE_XCHB_XL_MM_4)
				begin
					newacc8[0] = mem8[4];
					result8[4] = acc8[0];
				end
				else if (opcode == OPCODE_XCHB_XL_MM_5)
				begin
					newacc8[0] = mem8[5];
					result8[5] = acc8[0];
				end
				else if (opcode == OPCODE_XCHB_XL_MM_6)
				begin
					newacc8[0] = mem8[6];
					result8[6] = acc8[0];
				end
				else if (opcode == OPCODE_XCHB_XL_MM_7)
				begin
					newacc8[0] = mem8[7];
					result8[7] = acc8[0];
				end
				regwrite_data = {newacc8, newacc8};
				regwrite_addr = acc8_addr;
				regwrite_en = acc8_en;
				next_f[FLAG_Z] = !newacc8[0];
				memwrite_data = result8;
				memwrite_addr = inst[23:8];
				memwrite_en = 2'b01;
			end
`endif
			else if(opcode == OPCODE_LDW_Y_SP)
			begin
				if(swapop)
					next_sp = y;
				else
				begin
					regwrite_data = sp;
					regwrite_addr = acc16_addr;
					regwrite_en = 2'b11;
				end
			end
`ifndef F8L
			else if(opcode == OPCODE_LDW_ISPREL_Y)
			begin
				memwrite_data = acc16;
				memwrite_addr = mem16;
				memwrite_en = 2'b11;
			end
`endif
			else if(opcode == OPCODE_LD_XL_IMMD)
			begin
				regwrite_data = {imm8, imm8};
				regwrite_addr = acc8_addr;
				regwrite_en = acc8_en;
			end
			else if(opcode_is_ld_xl_mem(opcode))
			begin
				regwrite_data = {mem8, mem8};
				regwrite_addr = acc8_addr;
				regwrite_en = acc8_en;
				next_f = {3'b000, f[FLAG_O], !(|mem8), mem8[7], f[FLAG_C], f[FLAG_H]};
			end
			else if(opcode == OPCODE_LD_XL_XH)
			begin
				if(swapop)
				begin
					regwrite_data = {x[7:0], 8'hxx};
					regwrite_addr = 0;
					regwrite_en = 2'b10;
				end
				else
				begin
					regwrite_data = {x[15:8], x[15:8]};
					regwrite_addr = acc8_addr;
					regwrite_en = acc8_en;
				end
			end
			else if(opcode == OPCODE_LD_XL_YL)
			begin
				if(swapop)
				begin
					regwrite_data = {8'hxx, x[7:0]};
					regwrite_addr = 1;
					regwrite_en = 2'b01;
				end
				else
				begin
					regwrite_data = {y[7:0], y[7:0]};
					regwrite_addr = acc8_addr;
					regwrite_en = acc8_en;
				end
			end
			else if(opcode == OPCODE_LD_XL_YH)
			begin
				if(swapop)
				begin
					regwrite_data = {x[7:0], 8'hxx};
					regwrite_addr = 1;
					regwrite_en = 2'b10;
				end
				else
				begin
					regwrite_data = {y[15:8], y[15:8]};
					regwrite_addr = acc8_addr;
					regwrite_en = acc8_en;
				end
			end
			else if(opcode == OPCODE_LD_XL_ZL)
			begin
				if(swapop)
				begin
					regwrite_data = {8'hxx, x[7:0]};
					regwrite_addr = 2;
					regwrite_en = 2'b01;
				end
				else
				begin
					regwrite_data = {z[7:0], z[7:0]};
					regwrite_addr = acc8_addr;
					regwrite_en = acc8_en;
				end
			end
			else if(opcode == OPCODE_LD_XL_ZH)
			begin
				if(swapop)
				begin
					regwrite_data = {x[7:0], 8'hxx};
					regwrite_addr = 2;
					regwrite_en = 2'b10;
				end
				else
				begin
					regwrite_data = {z[15:8], z[15:8]};
					regwrite_addr = acc8_addr;
					regwrite_en = acc8_en;
				end
			end
			else if(opcode == OPCODE_LD_DIR_XL)
			begin
				memwrite_data = {8'hxx, acc8};
				memwrite_addr = inst[23:8];
				memwrite_en = 2'b01;
			end
			else if(opcode == OPCODE_LD_SPREL_XL)
			begin
				memwrite_data = {8'hxx, acc8};
				memwrite_addr = sp + {8'h00, inst[15:8]};
				memwrite_en = 2'b01;
			end
			else if(opcode == OPCODE_LD_ZREL_XL)
			begin
				memwrite_data = {8'hxx, acc8};
				memwrite_addr = z + inst[23:8];
				memwrite_en = 2'b01;
			end
			else if(opcode == OPCODE_LD_IY_XL)
			begin
				memwrite_data = {8'hxx, acc8};
				memwrite_addr = acc16;
				memwrite_en = 2'b01;
			end
`ifndef F8L
			else if(opcode == OPCODE_LD_YREL_XL)
			begin
				memwrite_data = {8'hxx, acc8};
				memwrite_addr = y + {8'h00, inst[15:8]};
				memwrite_en = 2'b01;
			end
`endif
			else if(opcode_is_push(opcode))
			begin
				memwrite_data = {8'hxx, op8};
				memwrite_addr = sp - 1;
				memwrite_en = 2'b01;
				if (opcode == OPCODE_PUSH_IMMD)
					next_f = f;
				next_sp = sp - 1;
			end
`ifndef F8L
			else if(opcode == OPCODE_XCH_XL_SPREL)
			begin
				memwrite_data = {8'hxx, acc8};
				memwrite_addr = sp + {8'h00, inst[15:8]};
				memwrite_en = 2'b01;
				regwrite_data = {mem8, mem8};
				regwrite_addr = acc8_addr;
				regwrite_en = acc8_en;
			end
`endif
			else if(opcode == OPCODE_XCH_XL_IY)
			begin
				memwrite_data = {8'hxx, acc8};
				memwrite_addr = acc16;
				memwrite_en = 2'b01;
				regwrite_data = {mem8, mem8};
				regwrite_addr = acc8_addr;
				regwrite_en = acc8_en;
			end
			else if(opcode == OPCODE_XCH_YL_YH)
			begin
				regwrite_data = {acc16[7:0], acc16[15:8]};
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
			end
`ifndef F8L
			else if(opcode == OPCODE_ROT_XL_IMMD)
			begin
				//logic [7:0] result8;
				result8 = rot(acc8, imm8[2:0]);
				regwrite_data = {result8, result8};
				regwrite_addr = acc8_addr;
				regwrite_en = acc8_en;
				next_f[FLAG_Z] = !result8;
				next_f[FLAG_N] = result8[7];
			end
`endif
			else if(opcode == OPCODE_POP_XL)
			begin
				regwrite_data = {mem8, mem8};
				regwrite_addr = acc8_addr;
				regwrite_en = acc8_en;
				next_sp = sp + 1;
			end
			else if(opcode == OPCODE_CAX_IY_ZL_XL)
			begin
				if(mem8 == z[7:0])
				begin
					memwrite_data = {8'hxx, acc8};
					memwrite_addr = y;
					memwrite_en = 2'b01;
					next_f[FLAG_Z] = 1;
				end
				else
				begin
					regwrite_data = mem8;
					regwrite_addr = 2;
					regwrite_en = 2'b01;
					next_f[FLAG_Z] = 0;
				end
			end
			else if(opcode_is_16_1(opcode) && !opcode_is_pushw(opcode)
`ifndef F8L
				|| opcode_is_16_2(opcode)
`endif
				)
			begin
				//logic[15:0] result16;
				//result16 = 'x;
`ifndef F8L
				if(opcode_is_subw(opcode))
				begin
					//addsub_result_t addsub_result;
					if (swapop)
						addsub_result = addsub (~acc16, op16, 1, 1);
					else
						addsub_result = addsub (acc16, ~op16, 1, 1);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
					result16 = addsub_result.result;
				end
				if(opcode_is_sbcw(opcode))
				begin
					//addsub_result_t addsub_result;
					if (swapop)
						addsub_result = addsub (~acc16, op16, f[FLAG_C], 1);
					else
						addsub_result = addsub (acc16, ~op16, f[FLAG_C], 1);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
					result16 = addsub_result.result;
				end
				else if(opcode_is_addw(opcode))
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub (acc16, op16, 0, 1);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
					result16 = addsub_result.result;
				end
				else if(opcode_is_adcw(opcode))
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub (acc16, op16, f[FLAG_C], 1);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
					result16 = addsub_result.result;
				end
				else if(opcode_is_orw(opcode))
				begin
					result16 = acc16 | op16;
					next_f = {3'b000, ^result16, !(|result16), result16[15], f[FLAG_C], f[FLAG_H]};
				end
				else if(opcode_is_xorw(opcode))
				begin
					result16 = acc16 ^ op16;
					next_f = {3'b000, ^result16, !(|result16), result16[15], f[FLAG_C], f[FLAG_H]};
				end
				else
`endif
				if(opcode_is_clrw(opcode))
				begin
					result16 = 16'h0000;
				end
				else if(opcode_is_tstw(opcode))
				begin
					next_f = {3'b000, ^op16, !(|op16), op16[15], 1'b1, f[FLAG_H]};
				end
				else if(opcode_is_incw(opcode))
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub (op16, 16'h0001, 0, 1);
`ifndef F8L
					if (opcode != OPCODE_INCNW_Y)
`endif
						next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
					result16 = addsub_result.result;
				end
`ifndef F8L
				else if(opcode_is_adcw0(opcode))
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub (op16, 16'h0000, f[FLAG_C], 1);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
					result16 = addsub_result.result;
				end
				else if(opcode_is_sbcw0(opcode))
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub (op16, 16'hffff, f[FLAG_C], 1);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
					result16 = addsub_result.result;
				end
				else if(opcode == OPCODE_SRLW_Y)
				begin
					result16 = {1'b0, op16[15:1]};
					next_f[FLAG_Z] = !result16;
					next_f[FLAG_C] = op16[0];
				end
				else if(opcode == OPCODE_SLLW_Y)
				begin
					result16 = {op16[14:0], 1'b0};
					next_f[FLAG_Z] = !result16;
					next_f[FLAG_C] = op16[15];
				end
				else if(opcode_is_rrcw(opcode))
				begin
					result16 = {f[FLAG_C], op16[15:1]};
					next_f[FLAG_Z] = !result16;
					next_f[FLAG_C] = op16[0];
				end
				else if(opcode_is_rlcw(opcode))
				begin
					result16 = {op16[14:0], f[FLAG_C]};
					next_f[FLAG_Z] = !result16;
					next_f[FLAG_C] = op16[15];
				end
				else if(opcode == OPCODE_SRAW_Y)
				begin
					result16 = {op16[15], op16[15:1]};
					next_f[FLAG_Z] = !result16;
					next_f[FLAG_C] = op16[0];
				end
				else if(opcode == OPCODE_DECW_SPREL)
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub (op16, 16'hfffe, 1, 1);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
					result16 = addsub_result.result;
				end
				else if(opcode == OPCODE_NEGW_Y)
				begin
					//addsub_result_t addsub_result;
					addsub_result = addsub (16'h0000, ~op16, 1, 1);
					next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
					result16 = addsub_result.result;
				end
				else if(opcode == OPCODE_BOOLW_Y)
				begin
					result16 = |op16;
					next_f[FLAG_Z] = !result16;
				end
`endif

				if (!swapop && !opcode_is_16_1_mem(opcode) && !opcode_is_tstw(opcode))
				begin
					regwrite_data = result16;
					regwrite_addr = acc16_addr;
					regwrite_en = 2'b11;
				end
				else if (!opcode_is_tstw(opcode) && !opcode_is_16_2_immd(opcode))
				begin
					if(opcode_is_16_2_mem(opcode) || opcode_is_16_1_mem(opcode))
					begin
						memwrite_data = result16;
						memwrite_en = 2'b11;
					end
					else
					begin
						regwrite_data = result16;
						regwrite_addr = op16_addr;
						regwrite_en = 2'b11;
					end
				end
			end
			else if(opcode_is_pushw(opcode))
			begin
				memwrite_data = op16;
				memwrite_addr = sp - 2;
				memwrite_en = 2'b11;
				next_sp = sp - 2;
			end
			else if(opcode == OPCODE_MSK_IY_XL_IMMD)
			begin
				memwrite_data = {8'hxx, acc8 & imm8 | mem8 & ~imm8};
				memwrite_addr = acc16;
				memwrite_en = 2'b01;
				next_f[FLAG_Z] = !(mem8 & ~imm8);
			end
`ifndef F8L
			else if(opcode == OPCODE_MUL_Y)
			begin
				regwrite_data = mad(acc16[15:8], acc16[7:0], 0, 0);
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_f[FLAG_Z] = !(|regwrite_data);
				next_f[FLAG_N] = regwrite_data[15];
				next_f[FLAG_C] = 0;
			end
`endif
			else if(opcode == OPCODE_RET)
			begin
				next_sp = sp + 2;
			end
			else if(opcode == OPCODE_RETI)
			begin
				next_sp = sp + 2;
				next_f = f;
			end
`ifndef F8L
			else if(opcode_is_mad(opcode))
			begin
				regwrite_data = mad(mem8, y[7:0], x[15:8], f[FLAG_C]);
				regwrite_addr = 0;
				regwrite_en = 2'b11;
				next_f[FLAG_Z] = !(|regwrite_data);
				next_f[FLAG_N] = regwrite_data[15];
			end
`endif
			else if(opcode == OPCODE_LDW_Y_IMMD)
			begin
				regwrite_data = imm16;
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_f[FLAG_Z] = !(|regwrite_data);
				next_f[FLAG_N] = regwrite_data[15];
			end
			else if(opcode == OPCODE_LDW_Y_DIR)
			begin
				regwrite_data = mem16;
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_f[FLAG_Z] = !(|regwrite_data);
				next_f[FLAG_N] = regwrite_data[15];
			end
			else if(opcode == OPCODE_LDW_Y_SPREL || opcode == OPCODE_LDW_Y_ZREL || opcode == OPCODE_LDW_Y_YREL || opcode == OPCODE_LDW_Y_IY)
			begin
				regwrite_data = mem16;
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_f[FLAG_Z] = !(|regwrite_data);
				next_f[FLAG_N] = regwrite_data[15];
			end
			else if(opcode == OPCODE_LDW_Y_X)
			begin
				regwrite_data = op16;
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
			end
			else if(opcode == OPCODE_LDW_Y_D)
			begin
				regwrite_data = {{8{imm8[7]}}, imm8};
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_f[FLAG_Z] = !(|regwrite_data);
				next_f[FLAG_N] = regwrite_data[15];
			end
			else if(opcode == OPCODE_LDW_DIR_Y || opcode == OPCODE_LDW_SPREL_Y || opcode == OPCODE_LDW_ZREL_Y)
			begin
				memwrite_data = acc16;
				memwrite_en = 2'b11;
			end
			else if(opcode == OPCODE_LDW_X_Y)
			begin
				regwrite_data = acc16;
				regwrite_addr = op16_addr;
				regwrite_en = 2'b11;
			end
			else if(opcode == OPCODE_LDW_Z_Y)
			begin
				if(swapop)
				begin
					regwrite_data = z;
					regwrite_addr = 1;
				end
				else
				begin
					regwrite_data = y;
					regwrite_addr = 2;
				end
				regwrite_en = 2'b11;
			end
			else if(opcode == OPCODE_LDW_IY_X)
			begin
				memwrite_data = op16;
				memwrite_addr = acc16;
				memwrite_en = 2'b11;
			end
`ifndef F8L
			else if(opcode == OPCODE_LDW_YREL_X)
			begin
				memwrite_data = op16;
				memwrite_addr = y + {8'h00, inst[15:8]};
				memwrite_en = 2'b11;
			end
`endif
			else if(opcode == OPCODE_JR_D)
			begin
				next_f = f;
			end
`ifndef F8L
			else if(opcode == OPCODE_DNJNZ_YH_D)
			begin
				//addsub_result_t addsub_result;
				addsub_result = addsub ({8'h00, acc16[15:8]}, 16'h00fe, 1, 0);
				regwrite_data = {addsub_result.result[7:0], 8'h00};
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b10;
				next_f[FLAG_Z] = addsub_result.z;
				next_f[FLAG_N] = addsub_result.n;
			end
			else if(opcode == OPCODE_SLLW_Y_XL)
			begin
				regwrite_data = acc16 << acc8[3:0];
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_f[FLAG_Z] = !(|regwrite_data);
				next_f[FLAG_N] = regwrite_data[15];
			end
`endif
			else if(opcode == OPCODE_PUSHW_IMMD)
			begin
				memwrite_data = imm16;
				memwrite_addr = sp - 2;
				memwrite_en = 2'b11;
				next_sp = sp - 2;
			end
			else if(opcode == OPCODE_POPW_Y)
			begin
				regwrite_data = memop;
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_sp = sp + 2;
			end
			else if(opcode == OPCODE_ADDW_SP_D)
			begin
				//addsub_result_t addsub_result;
				addsub_result = addsub (sp, {{8{imm8[7]}}, imm8}, 0, 1);
				next_f = f;
				next_sp = addsub_result.result;
			end
			else if(opcode == OPCODE_ADDW_Y_D)
			begin
				//addsub_result_t addsub_result;
				addsub_result = addsub (acc16, {{8{imm8[7]}}, imm8}, 0, 1);
				next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
				regwrite_data = addsub_result.result;
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
			end
			else if(opcode == OPCODE_XCH_F_SPREL)
			begin
				memwrite_data = f;
				memwrite_en = 2'b01;
				next_f = memop[7:0];
			end
`ifndef F8L
			else if(opcode == OPCODE_SEX_Y_XL)
			begin
				regwrite_data = {{8{acc8[7]}}, acc8};
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_f[FLAG_Z] = !(|regwrite_data);
				next_f[FLAG_N] = regwrite_data[15];
			end
			else if(opcode == OPCODE_ZEX_Y_XL)
			begin
				regwrite_data = {8'h00, acc8};
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_f[FLAG_Z] = !(|regwrite_data);
			end
`endif
			else if(opcode == OPCODE_XCHW_X_IY)
			begin
				regwrite_data = mem16;
				regwrite_addr = op16_addr;
				regwrite_en = 2'b11;
				memwrite_data = op16;
				memwrite_addr = acc16;
				memwrite_en = 2'b11;
			end
`ifndef F8L
			else if(opcode == OPCODE_XCHW_Y_SPREL)
			begin
				regwrite_data = mem16;
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				memwrite_data = acc16;
				memwrite_en = 2'b11;
			end
			else if(opcode == OPCODE_CPW_Y_IMMD)
			begin
				//addsub_result_t addsub_result;
				if (!swapop)
					addsub_result = addsub (acc16, ~imm16, 1, 1);
				else
					addsub_result = addsub (~acc16, imm16, 1, 1);
				next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
			end
`endif
			else if(opcode == OPCODE_CAXW_IY_Z_X)
			begin
				if(mem16 == z)
				begin
					memwrite_data = x;
					memwrite_addr = y;
					memwrite_en = 2'b11;
					next_f[FLAG_Z] = 1;
				end
				else
				begin
					regwrite_data = mem16;
					regwrite_addr = 2;
					regwrite_en = 2'b11;
					next_f[FLAG_Z] = 0;
				end
			end
`ifndef F8L
			else if(opcode == OPCODE_LDI_IZ_IY)
			begin
				regwrite_addr = 2;
				regwrite_en = 2'b11;
				memwrite_addr = z;
				regwrite_data = z + 1;
				memwrite_data = mem8;
				memwrite_en = 2'b01;
				next_f = {3'b000, f[FLAG_O], !(|mem8), mem8[7], f[FLAG_C], f[FLAG_H]};
			end
			else if(opcode == OPCODE_LDWI_IZ_IY)
			begin
				regwrite_addr = 2;
				regwrite_en = 2'b11;
				memwrite_addr = z;
				regwrite_data = z + 2;
				memwrite_data = mem16;
				memwrite_en = 2'b11;
				next_f = {3'b000, f[FLAG_O], !(|mem16), mem16[15], f[FLAG_C], f[FLAG_H]};
			end
`endif
		end
		else
			next_f = f;
	end
endmodule

`end_keywords

