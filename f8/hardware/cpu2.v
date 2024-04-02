`begin_keywords "1800-2009"

`include "opcode.v"
`include "alu2.v"

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
	input logic clk, reset);
	logic [15:0] gpregs[3];

	assign x = gpregs[0];
	assign y = gpregs[1];
	assign z = gpregs[2];

	always @(posedge clk)
	begin
		if (write_en[0])
			gpregs[addr_in][7:0] = data_in[7:0];
		if (write_en[1])
			gpregs[addr_in][15:8] = data_in[15:8];
		if (reset)
			f = {3'b000, next_f[4:0]};
		else
			f = next_f;
		if (reset)
			pc = 16'h4000;
		else
			pc = next_pc;
		sp = next_sp;
		if (reset)
			oldinst = OPCODE_NOP;
		else
			oldinst = inst;
		if (reset)
			oldinst_valid = 0;
		else
			oldinst_valid = next_oldinst_valid;
		if (reset)
			oldinst_loadsop = 0;
		else
			oldinst_loadsop = next_oldinst_loadsop;
	end
endmodule

function automatic logic opcode_loads_upper(opcode_t opcode);
	return(0);
endfunction

function automatic logic opcode_loads_operand(opcode_t opcode);
	return(opcode_is_8_immd(opcode) || opcode_is_16_immd(opcode) || opcode == OPCODE_CALL_IMMD || opcode == OPCODE_RET || opcode == OPCODE_POPW_Y || opcode == OPCODE_XCH_F_SPREL || opcode == OPCODE_JP_IMMD || opcode == OPCODE_LDW_Y_D);
endfunction

typedef enum logic [1:0] {
	ACCSEL_XL_Y = 2'b00,
	ACCSEL_XH_Y = 2'b01,
	ACCSEL_YL_Z = 2'b10,
	ACCSEL_ZL_X = 2'b11
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
	assign accsel = accsel_t'(f[7:6]);
	assign acc16 = (accsel == ACCSEL_YL_Z) ? z :
			(accsel == ACCSEL_ZL_X) ? x :
			y;

	logic loading_upper_inst, loading_operand, inst_upper_valid, execute;

	// Memory read
	always_comb
	begin
		logic [15:0] mem_read_addr;
		if(opcode_is_8_immd(opcode) || opcode_is_16_immd(opcode) || opcode == OPCODE_CALL_IMMD || opcode == OPCODE_JP_IMMD || opcode == OPCODE_LDW_Y_D)
			memop_addr = pc + 1;
		else if(opcode == OPCODE_XCH_F_SPREL)
			memop_addr = sp + inst[15:8];
		else if(opcode == OPCODE_RET || opcode == OPCODE_POPW_Y)
			memop_addr = sp;
		else
			memop_addr = 'x;
		if(loading_upper_inst)
		begin
			mem_read_addr = pc + 2;
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

	// Instruction handling
	assign loading_upper_inst = opcode_loads_upper (opcode) && !oldinst_valid;
	assign inst_upper_valid = oldinst_valid;
	assign next_oldinst_loadsop = opcode_loads_operand(opcode) && !execute;
	assign execute = !reset && (!opcode_loads_upper(opcode) || inst_upper_valid) && (!opcode_loads_operand(opcode) || oldinst_loadsop);
	assign next_oldinst_valid = loading_upper_inst && !execute;
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
		accsel = accsel_t'(f[7:6]);
		if(!execute)
			next_pc_noint = pc;
		else if(opcode == OPCODE_CALL_IMMD || opcode == OPCODE_JP_IMMD)
			next_pc_noint = memop;
		else if(opcode == OPCODE_CALL_Y || opcode == OPCODE_JP_Y)
			next_pc_noint = acc16;
		else if(opcode == OPCODE_RET)
			next_pc_noint = memop;
		else if(opcode == OPCODE_JR_D ||
			opcode == OPCODE_JRC_D && f[FLAG_C] || opcode == OPCODE_JRNC_D && !f[FLAG_C] ||
			opcode == OPCODE_JRN_D && f[FLAG_N] || opcode == OPCODE_JRNN_D && !f[FLAG_N] ||
			opcode == OPCODE_JRZ_D && f[FLAG_Z] || opcode == OPCODE_JRNZ_D && !f[FLAG_Z] ||
			opcode == OPCODE_JRO_D && f[FLAG_O] || opcode == OPCODE_JRNO_D && !f[FLAG_O] ||
			opcode == OPCODE_JRSGE_D && !(f[FLAG_N] ^ f[FLAG_O]) || opcode == OPCODE_JRSLT_D && (f[FLAG_N] ^ f[FLAG_O]) ||
			opcode == OPCODE_JRSGT_D && !(f[FLAG_Z] || (f[FLAG_N] ^ f[FLAG_O])) || opcode == OPCODE_JRSLE_D && (f[FLAG_Z] || (f[FLAG_N] ^ f[FLAG_O])) ||
			opcode == OPCODE_JRGT_D && (f[FLAG_C] && !f[FLAG_Z]) || opcode == OPCODE_JRLE_D && (!f[FLAG_C] || f[FLAG_Z]) ||
			opcode == OPCODE_DNJNZ_YH_D && ((accsel == ACCSEL_ZL_X) ? x[15:8] : (accsel == ACCSEL_YL_Z) ? z[15:8] : y[15:8]) == 8'h01)
			next_pc_noint = signed'(pc) + signed'(inst[15:8]);
		else
			next_pc_noint = pc + opcode_instsize(opcode);
		next_pc = next_pc_noint;
	end

	// Instruction execution
	assign trap = !reset && (opcode == OPCODE_TRAP);
	always_comb
	begin
		
		logic swapop;
		logic [1:0] acc8_addr, acc16_addr;
		logic [1:0] acc8_en;
		logic [7:0] acc8, imm8;
		logic [15:0] imm16;
		swapop = f[5];
		acc8_addr = (accsel == ACCSEL_ZL_X) ? 2 : (accsel == ACCSEL_YL_Z) ? 1 : 0;
		acc16_addr = (accsel == ACCSEL_ZL_X) ? 0 : (accsel == ACCSEL_YL_Z) ? 2 : 1;
		acc8_en = (accsel == ACCSEL_XH_Y) ? 2'b10 : 2'b01;
		acc8 = (accsel == ACCSEL_XH_Y) ? x[15:8] :
			(accsel == ACCSEL_YL_Z) ? y[7:0] :
			(accsel == ACCSEL_ZL_X) ? z[7:0] :
			x[7:0];
		imm8 = memop[7:0];
		imm16 = memop[15:0];
		regwrite_data = 'x;
		regwrite_addr = 'x;
		regwrite_en = 2'b00;
		next_f = {3'b000, f[4:0]};
		memwrite_en = 2'b00;
		next_sp = sp;
		if(execute)
		begin
			if(opcode == OPCODE_SWAPOP)
				next_f |= 1 << FLAG_5;
			else if(opcode == OPCODE_ALTACC1)
				next_f[7:6] = 1;
			else if(opcode == OPCODE_ALTACC2)
				next_f[7:6] = 2;
			else if(opcode == OPCODE_ALTACC3)
				next_f[7:6] = 3;
			else if(opcode == OPCODE_CP_XL_IMMD)
			begin
				addsub_result_t addsub_result;
				if (!swapop)
					addsub_result = addsub ({8'h00, acc8}, {8'h00, ~imm8}, 1, 0);
				else
					addsub_result = addsub ({8'h00, ~acc8}, {8'h00, imm8}, 1, 0);
				next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, addsub_result.h};
			end
			else if(opcode == OPCODE_CLR_XL)
			begin
				regwrite_data = {16'h0000};
				regwrite_addr = acc8_addr;
				regwrite_en = acc8_en;
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
			else if(opcode == OPCODE_LD_XL_IMMD)
			begin
				regwrite_data = {imm8, imm8};
				regwrite_addr = acc8_addr;
				regwrite_en = acc8_en;
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
			else if(opcode == OPCODE_PUSH_IMMD)
			begin
				memwrite_data = {8'hxx, imm8};
				memwrite_addr = sp - 1;
				memwrite_en = 2'b01;
				next_f = f;
				next_sp = sp - 1;
			end
			else if(opcode == OPCODE_RET)
			begin
				next_sp = sp + 2;
			end
			else if(opcode == OPCODE_LDW_Y_IMMD)
			begin
				regwrite_data = memop;
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_f[FLAG_Z] = !(|regwrite_data);
				next_f[FLAG_N] = regwrite_data[15];
			end
			else if(opcode == OPCODE_LDW_Y_X)
			begin
				regwrite_data = x;
				regwrite_addr = acc16;
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
			else if(opcode == OPCODE_LDW_X_Y)
			begin
				regwrite_data = y;
				regwrite_addr = 0;
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
			else if(opcode == OPCODE_POPW_Y)
			begin
				regwrite_data = memop;
				regwrite_addr = acc16_addr;
				regwrite_en = 2'b11;
				next_sp = sp + 2;
			end
			else if(opcode == OPCODE_XCH_F_SPREL)
			begin
				memwrite_data = f;
				memwrite_addr = sp - 1;
				memwrite_en = 2'b01;
				next_f = memop[7:0];
			end
			else if(opcode == OPCODE_CPW_Y_IMMD)
			begin
				addsub_result_t addsub_result;
				if (!swapop)
					addsub_result = addsub (acc16, ~imm16, 1, 1);
				else
					addsub_result = addsub (~acc16, imm16, 1, 1);
				next_f = {3'b000, addsub_result.o, addsub_result.z, addsub_result.n, addsub_result.c, f[FLAG_H]};
			end
		end
		else
			next_f = f;
	end
endmodule

`end_keywords

