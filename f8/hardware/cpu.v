`begin_keywords "1800-2009"

`include "alu.v"
`include "opcode.v"

module regfile(output logic [15:0] x, y, z, output logic [15:0] next_x, next_y, next_z, input logic [1:0] addr_in, input logic [15:0] data_in, input logic [1:0] write_en, input logic clk);
	logic [15:0] regs[3];

	assign x = regs[0];
	assign y = regs[1];
	assign z = regs[2];

	assign next_x = {(addr_in == 0 && write_en[1]) ? data_in[15:8]: regs[0][15:8], (addr_in == 0 && write_en[0]) ? data_in[7:0]: regs[0][7:0]};
	assign next_y = {(addr_in == 1 && write_en[1]) ? data_in[15:8]: regs[1][15:8], (addr_in == 1 && write_en[0]) ? data_in[7:0]: regs[1][7:0]};
	assign next_z = {(addr_in == 2 && write_en[1]) ? data_in[15:8]: regs[2][15:8], (addr_in == 2 && write_en[0]) ? data_in[7:0]: regs[2][7:0]};

	always @(posedge clk)
	begin
		if (write_en[0])
			regs[addr_in][7:0] = data_in[7:0];
		if (write_en[1])
			regs[addr_in][15:8] = data_in[15:8];
	end
endmodule

typedef enum logic [1:0] {
	ACCSEL_XL_Y = 2'b00,
	ACCSEL_XH_Y = 2'b01,
	ACCSEL_YL_Z = 2'b10,
	ACCSEL_ZL_X = 2'b11
} accsel_t;

module cpu(iread_addr, iread_data, iread_valid, dread_addr, dread_data, dwrite_addr, dwrite_data, dwrite_en, clk, reset, interrupt, trap);
	output [15:0] iread_addr;
	input [23:0] iread_data;
	input iread_valid;
	output logic [15:0] dread_addr;
	input [15 : 0] dread_data;
	output [15 : 0] dwrite_addr;
	output [15 : 0] dwrite_data;
	output logic [1:0] dwrite_en;
	input clk, reset, interrupt;
	output trap;

	wire [23:0] next_inst;
	wire [7:0] next_opcode;
	wire nextinst_valid;
	reg [23:0] inst;
	wire [7:0] opcode;

	logic [15:0] op0, op1, op2;
	wire [15:0] result_reg, result_mem;
	aluinst_t aluinst;
	accsel_t accsel_in, next_accsel_in;
	logic h_in, c_in, swapop_in, c_out, z_out, n_out, o_out, h_out;
	reg [7:0] flags;

	wire interupt_start;
	reg interrupt_active;

	alu alu(.*);

	wire [15:0] x, y, z;
	wire [15:0] next_x, next_y, next_z;
	logic [7:0] next_flags;
	logic [1:0] regwrite_en;
	logic [1:0] regwrite_addr;
	logic [15:0] regwrite_data;

	logic [15:0] sp, next_sp;
	logic [15:0] pc, old_pc, next_pc_noint, next_pc;

	regfile regfile(.addr_in(regwrite_addr), .data_in(regwrite_data), .write_en(regwrite_en), .*);

	assign next_opcode = next_inst[7:0];
	assign opcode = interrupt_start ? OPCODE_NOP : inst[7:0];

	// Handle program counter
	always @(posedge clk)
	begin
		pc <= next_pc;
		/*if (next_opcode == OPCODE_RETI && opcode != OPCODE_RETI) // Do not update old_pc during first cycle of reti (otherwise reti can get executed twice, after an interrupt that was pending at the end of the previous interrupt handler).
			old_pc = old_pc;
		else*/
			old_pc = pc;
		sp <= next_sp;
	end

	always @(posedge clk)
		inst <= next_inst;

	// Interrupts
	assign interrupt_start = interrupt && !interrupt_active;
	always @(posedge clk)
	begin
		if(reset)
			interrupt_active = 0;
		else if(interrupt_start && !interrupt_active)
			interrupt_active = 1;
		else if(interrupt_active && opcode == OPCODE_RETI && next_opcode != OPCODE_RETI)
			interrupt_active = 0;
	end

	always_comb
	begin
		if(next_opcode == OPCODE_JP_IMMD || next_opcode == OPCODE_CALL_IMMD)
			next_pc_noint = next_inst[31:8];
		else if(next_opcode == OPCODE_JP_Y || next_opcode == OPCODE_CALL_Y)
			next_pc_noint = (next_accsel_in == ACCSEL_ZL_X) ? next_x : (next_accsel_in == ACCSEL_YL_Z) ? next_z : next_y;
		else if(next_opcode == OPCODE_JR_D ||
			next_opcode == OPCODE_JRZ_D && next_flags[3] || next_opcode == OPCODE_JRNZ_D && !next_flags[3] ||
			next_opcode == OPCODE_JRC_D && next_flags[1] || next_opcode == OPCODE_JRNC_D && !next_flags[1] ||
			next_opcode == OPCODE_JRN_D && next_flags[2] || next_opcode == OPCODE_JRNN_D && !next_flags[2] ||
			next_opcode == OPCODE_JRO_D && next_flags[4] || next_opcode == OPCODE_JRNO_D && !next_flags[4] ||
			next_opcode == OPCODE_JRSGE_D && !(next_flags[2] ^ next_flags[4]) || next_opcode == OPCODE_JRSLT_D && (next_flags[2] ^ next_flags[4]) ||
			next_opcode == OPCODE_JRSGT_D && !(next_flags[3] || (next_flags[2] ^ next_flags[4])) || next_opcode == OPCODE_JRSLE_D && (next_flags[3] || (next_flags[2] ^ next_flags[4])) ||
			next_opcode == OPCODE_JRGT_D && (next_flags[1] && !next_flags[3]) || next_opcode == OPCODE_JRLE_D && (!next_flags[1] || next_flags[3]) ||
			next_opcode == OPCODE_DNJNZ_YH_D && ((next_accsel_in == ACCSEL_ZL_X) ? next_x[15:8] : (next_accsel_in == ACCSEL_YL_Z) ? next_z[15:8] : next_y[15:8]) == 8'h01)
			next_pc_noint = signed'(pc) + signed'(next_inst[15:8]);
		else if(opcode_is_8_immd(next_opcode) || opcode_is_sprel(next_opcode) || opcode_is_yrel(next_opcode) || opcode_is_jr_d(next_opcode) || next_opcode == OPCODE_LDW_Y_D || next_opcode == OPCODE_ADDW_Y_D || next_opcode == OPCODE_ADDW_SP_D)
			next_pc_noint = pc + 2;
		else if(opcode_is_16_immd(next_opcode) || opcode_is_dir(next_opcode) || opcode_is_zrel(next_opcode))
			next_pc_noint = pc + 3;
		else if(next_opcode == OPCODE_RET && next_flags[5] || next_opcode == OPCODE_RETI && opcode == OPCODE_RETI) // Second cycle of ret / reti.
			next_pc_noint = dread_data;
		else if(next_opcode == OPCODE_RET || next_opcode == OPCODE_RETI)
			next_pc_noint = pc;
		else
			next_pc_noint = pc + 1;
			
		if(reset)
			next_pc = 16'h4000;
		else if(interrupt_start)
			next_pc = 16'h4004;
		else
			next_pc = next_pc_noint;
	end

	always_comb
	begin
		if(interrupt_start)
			next_sp = sp - 2;
		else if(opcode_is_push(opcode))
			next_sp = sp - 1;
		else if(opcode == OPCODE_POP_XL)
			next_sp = sp + 1;
		else if(opcode_is_pushw(opcode) || opcode == OPCODE_CALL_IMMD || opcode == OPCODE_CALL_Y)
			next_sp = sp - 2;
		else if(opcode == OPCODE_POPW_Y)
			next_sp = sp + 2;
		else if(opcode == OPCODE_LDW_Y_SP && swapop_in || opcode == OPCODE_ADDW_SP_D)
			next_sp = result_reg;
		else if(opcode == OPCODE_RET && flags[5] || next_opcode == OPCODE_RETI && opcode == OPCODE_RETI)
			next_sp = sp + 2;
		else
			next_sp = sp;
	end

	assign iread_addr = next_pc;
	assign next_inst = interrupt_start ? {16'h0000, OPCODE_NOP} : iread_data;

	// Handle memory reads
	always_comb
	begin
		if(opcode_is_dir_read(next_opcode))
			dread_addr = next_inst[23:8];
		else if(opcode_is_sprel(next_opcode))
			dread_addr = next_sp + next_inst[15:8];
		else if(opcode_is_zrel(next_opcode))
			dread_addr = next_z + next_inst[23:8];
		else if(opcode_is_yrel(next_opcode))
			dread_addr = next_y + next_inst[15:8];
		else if(next_opcode == OPCODE_XCH_XL_IY || next_opcode == OPCODE_LD_XL_IY || next_opcode == OPCODE_CAX_IY_ZL_XL || next_opcode == OPCODE_CAXW_IY_Z_X || next_opcode == OPCODE_LDW_Y_IY)
			dread_addr = next_y;
		else if(next_opcode == OPCODE_MSK_IY_XL_IMMD)
			dread_addr = (next_accsel_in == ACCSEL_ZL_X) ? next_x : (next_accsel_in == ACCSEL_YL_Z) ? next_z : next_y;
		else if(next_opcode == OPCODE_POP_XL || next_opcode == OPCODE_POPW_Y || next_opcode == OPCODE_RET && !next_flags[5] || next_opcode == OPCODE_RETI && opcode != OPCODE_RETI)
			dread_addr = next_sp;
		else
			dread_addr = 'x;
	end

	assign swapop_in = flags[5];
	assign accsel_in = accsel_t'(flags[7:6]);
	assign next_accsel_in = accsel_t'(next_flags[7:6]);
	assign h_in = flags[0];
	assign c_in = flags[1];

	always_comb
	begin 
		if (interrupt_start)
			op0 = opcode == OPCODE_RETI ? next_pc_noint : old_pc;
		else if(opcode_is_8_2(opcode) || opcode_is_8_1_xl(opcode) || opcode_is_xchb(opcode) || opcode == OPCODE_ROT_XL_IMMD || opcode == OPCODE_XCH_XL_SPREL || opcode == OPCODE_XCH_XL_IY ||
			opcode == OPCODE_LD_DIR_XL ||opcode == OPCODE_LD_SPREL_XL || opcode == OPCODE_LD_ZREL_XL || opcode == OPCODE_LD_IY_XL || opcode == OPCODE_LD_YREL_XL ||
			opcode == OPCODE_SEX_Y_XL || opcode == OPCODE_ZEX_Y_XL)
			op0 = {8'bx,
				(accsel_in == ACCSEL_XH_Y) ? x[15:8] :
				(accsel_in == ACCSEL_YL_Z) ? y[7:0] :
				(accsel_in == ACCSEL_ZL_X) ? z[7:0] :
				x[7:0]};
		else if(opcode_is_8_1_zh(opcode))
			op0 = {8'bx, z[15:8]};
		else if(opcode_is_8_1_dir(opcode) || opcode_is_8_1_sprel(opcode) || opcode == OPCODE_MSK_IY_XL_IMMD || opcode == OPCODE_CAX_IY_ZL_XL || opcode == OPCODE_POP_XL || opcode == OPCODE_LD_XL_DIR || opcode == OPCODE_LD_XL_SPREL || opcode == OPCODE_LD_XL_ZREL || opcode == OPCODE_LD_XL_IY || opcode == OPCODE_LD_XL_YREL)
			op0 = {8'bx, dread_data[7:0]};
		else if(opcode == OPCODE_NEGW_Y)
			op0 = 0;
		else if(opcode_is_16_2(opcode) || opcode_is_16_1_y(opcode) || opcode == OPCODE_SLLW_Y_XL || opcode == OPCODE_LDW_Y_SP && swapop_in ||
			opcode == OPCODE_LDW_DIR_Y || opcode == OPCODE_LDW_SPREL_Y || opcode == OPCODE_LDW_ZREL_Y ||
			opcode == OPCODE_LDW_X_Y || opcode == OPCODE_LDW_Z_Y || opcode == OPCODE_CPW_Y_IMMD || opcode == OPCODE_ADDW_Y_D || opcode == OPCODE_LDW_ISPREL_Y || opcode == OPCODE_XCHW_Y_SPREL)
			op0 =
				(accsel_in == ACCSEL_ZL_X) ? x :
				(accsel_in == ACCSEL_YL_Z) ? z :
				y;
		else if ((opcode == OPCODE_LD_XL_XH || opcode == OPCODE_LD_XL_YL || opcode == OPCODE_LD_XL_YH || opcode == OPCODE_LD_XL_ZL || opcode == OPCODE_LD_XL_ZH) && swapop_in)
			op0 = {8'bx, x[7:0]};
		else if (opcode_is_mad(opcode) || opcode == OPCODE_LD_XL_XH && !swapop_in)
			op0 = {8'bx, x[15:8]};
		else if (opcode == OPCODE_LD_XL_YL && !swapop_in)
			op0 = {8'bx, y[7:0]};
		else if (opcode == OPCODE_LD_XL_YH && !swapop_in)
			op0 = {8'bx, y[15:8]};
		else if (opcode == OPCODE_LD_XL_ZL && !swapop_in)
			op0 = {8'bx, z[7:0]};
		else if (opcode == OPCODE_LD_XL_ZH && !swapop_in)
			op0 = {8'bx, z[15:8]};
		else if (opcode == OPCODE_XCH_F_SPREL)
			op0 = {8'bx, flags};
		else if (opcode == OPCODE_POPW_Y || opcode_is_16_1_dir(opcode) || opcode_is_16_1_sprel(opcode) || opcode_is_16_1_zrel(opcode) ||
			opcode == OPCODE_CAXW_IY_Z_X ||
			opcode == OPCODE_LDW_Y_DIR || opcode == OPCODE_LDW_Y_SPREL || opcode == OPCODE_LDW_Y_ZREL || opcode == OPCODE_LDW_Y_YREL || opcode == OPCODE_LDW_Y_IY)
			op0 = dread_data[15:0];
		else if(opcode == OPCODE_LD_XL_IMMD || opcode == OPCODE_LDW_Y_D || opcode == OPCODE_PUSH_IMMD)
			op0 = {8'bx, inst[15:8]};
		else if(opcode == OPCODE_LDW_Y_IMMD || opcode == OPCODE_PUSHW_IMMD)
			op0 = inst[23:8];
		else if(opcode == OPCODE_CALL_IMMD)
			op0 = old_pc + 3;
		else if(opcode == OPCODE_CALL_Y)
			op0 = old_pc + 1;
		else if(opcode == OPCODE_LDW_Y_SP || opcode == OPCODE_ADDW_SP_D)
			op0 = sp;
		else if(opcode == OPCODE_LDW_Y_X || opcode == OPCODE_LDW_IY_X || opcode == OPCODE_LDW_YREL_X)
			op0 = x;
		else if (opcode == OPCODE_DNJNZ_YH_D)
			op0 = {8'bx,
				(accsel_in == ACCSEL_ZL_X) ? x[15:8] :
				(accsel_in == ACCSEL_YL_Z) ? z[15:8] :
				y[15:8]};
		else
			op0 = 'x;

		if(opcode_is_8_2_immd(opcode) || opcode == OPCODE_ROT_XL_IMMD || opcode == OPCODE_ADDW_Y_D || opcode == OPCODE_ADDW_SP_D)
			op1 = {8'bx, inst[15:8]};
		else if(opcode_is_8_2_dir(opcode) || opcode_is_8_2_sprel(opcode) || opcode_is_8_2_zrel(opcode) || opcode_is_xchb(opcode) || opcode_is_mad(opcode) || opcode == OPCODE_XCH_XL_SPREL || opcode == OPCODE_XCH_XL_IY || opcode == OPCODE_XCH_F_SPREL)
			op1 = {8'bx, dread_data[7:0]};
		else if(opcode_is_8_2_zl(opcode))
			op1 = z[7:0];
		else if(opcode_is_8_2_xh(opcode))
			op1 = x[15:8];
		else if(opcode_is_8_2_yl(opcode))
			op1 = y[7:0];
		else if(opcode_is_8_2_yh(opcode))
			op1 = y[15:8];
		else if(opcode_is_16_2_immd(opcode) || opcode == OPCODE_CPW_Y_IMMD || opcode == OPCODE_MSK_IY_XL_IMMD)
			op1 = inst[31:8];
		else if(opcode_is_16_2_dir(opcode) || opcode_is_16_2_sprel(opcode) || opcode == OPCODE_LDW_ISPREL_Y || opcode == OPCODE_XCHW_Y_SPREL)
			op1 = dread_data[15:0];
		else if(opcode_is_16_2_x(opcode) || opcode == OPCODE_CAX_IY_ZL_XL || opcode == OPCODE_CAXW_IY_Z_X)
			op1 = x;
		else if (opcode == OPCODE_MUL_Y)
			op1 = {8'bx,
				(accsel_in == ACCSEL_ZL_X) ? x[15:8] :
				(accsel_in == ACCSEL_YL_Z) ? z[15:8] :
				y[15:8]};
		else if (opcode == OPCODE_NEGW_Y)
			op1 =
				(accsel_in == ACCSEL_ZL_X) ? x :
				(accsel_in == ACCSEL_YL_Z) ? z :
				y;
		else
			op1 = 'x;

		if(opcode == OPCODE_XCHB_XL_MM_0)
			op2 = 0;
		else if(opcode == OPCODE_XCHB_XL_MM_1)
			op2 = 1;
		else if(opcode == OPCODE_XCHB_XL_MM_2)
			op2 = 2;
		else if(opcode == OPCODE_XCHB_XL_MM_3)
			op2 = 3;
		else if(opcode == OPCODE_XCHB_XL_MM_4)
			op2 = 4;
		else if(opcode == OPCODE_XCHB_XL_MM_5)
			op2 = 5;
		else if(opcode == OPCODE_XCHB_XL_MM_6)
			op2 = 6;
		else if(opcode == OPCODE_XCHB_XL_MM_7)
			op2 = 7;
		else if (opcode == OPCODE_MUL_Y)
			op2 = {8'bx,
				(accsel_in == ACCSEL_ZL_X) ? x[7:0] :
				(accsel_in == ACCSEL_YL_Z) ? z[7:0] :
				y[7:0]};
		else if (opcode_is_mad(opcode))
			op2 = {8'bx, y[7:0]};
		else if (opcode == OPCODE_MSK_IY_XL_IMMD || opcode == OPCODE_SLLW_Y_XL)
			op2 = {8'bx,
				(accsel_in == ACCSEL_XH_Y) ? x[15:8] :
				(accsel_in == ACCSEL_YL_Z) ? y[7:0] :
				(accsel_in == ACCSEL_ZL_X) ? z[7:0] :
				x[7:0]};
		else if (opcode == OPCODE_CAX_IY_ZL_XL || opcode == OPCODE_CAXW_IY_Z_X)
			op2 = z;
		else
			op2 = 'x;

		if (interrupt_start)
			aluinst = ALUINST_PASSW0;
		else if (opcode_is_sub(opcode))
			aluinst = ALUINST_SUB;
		else if (opcode_is_sbc(opcode))
			aluinst = ALUINST_SBC;
		else if (opcode_is_add(opcode))
			aluinst = ALUINST_ADD;
		else if (opcode_is_adc(opcode))
			aluinst = ALUINST_ADC;
		else if (opcode_is_cp(opcode)) // Same operation as sub, but result is discarded.
			aluinst = ALUINST_SUB;
		else if (opcode_is_or(opcode))
			aluinst = ALUINST_OR;
		else if (opcode_is_and(opcode))
			aluinst = ALUINST_AND;
		else if (opcode_is_xor(opcode))
			aluinst = ALUINST_XOR;
		else if (opcode_is_srl(opcode))
			aluinst = ALUINST_SRL;
		else if (opcode_is_sll(opcode))
			aluinst = ALUINST_SLL;
		else if (opcode_is_rrc(opcode))
			aluinst = ALUINST_RRC;
		else if (opcode_is_rlc(opcode))
			aluinst = ALUINST_RLC;
		else if (opcode == OPCODE_SRA_XL)
			aluinst = ALUINST_SRA;
		else if (opcode_is_inc(opcode))
			aluinst = ALUINST_INC;
		else if (opcode_is_dec(opcode) || opcode == OPCODE_DNJNZ_YH_D)
			aluinst = ALUINST_DEC;
		else if (opcode_is_clr(opcode))
			aluinst = ALUINST_CLRW;
		else if (opcode_is_tst(opcode))
			aluinst = ALUINST_PASS0;
		else if (opcode == OPCODE_DAA_XL)
			aluinst = ALUINST_DAA;
		else if (opcode == OPCODE_BOOL_XL)
			aluinst = ALUINST_BOOL;
		else if (opcode == OPCODE_ROT_XL_IMMD)
			aluinst = ALUINST_ROT;
		else if (opcode_is_subw(opcode))
			aluinst = ALUINST_SUBW;
		else if (opcode_is_sbcw(opcode))
			aluinst = ALUINST_SBCW;
		else if (opcode_is_addw(opcode))
			aluinst = ALUINST_ADDW;
		else if (opcode_is_adcw(opcode))
			aluinst = ALUINST_ADCW;
		else if (opcode_is_orw(opcode))
			aluinst = ALUINST_ORW;
		else if (opcode_is_xchb(opcode))
			aluinst = ALUINST_XCHB;
		else if (opcode_is_clrw(opcode))
			aluinst = ALUINST_CLRW;
		else if (opcode_is_incw(opcode))
			aluinst = ALUINST_INCW;
		else if (opcode == OPCODE_DECW_SPREL)
			aluinst = ALUINST_DECW;
		else if (opcode_is_adcw0(opcode))
			aluinst = ALUINST_ADCW0;
		else if (opcode_is_sbcw0(opcode))
			aluinst = ALUINST_SBCW0;
		else if (opcode == OPCODE_ADDW_Y_D || opcode == OPCODE_ADDW_SP_D)
			aluinst = ALUINST_ADSW;
		else if (opcode == OPCODE_SRLW_Y)
			aluinst = ALUINST_SRLW;
		else if (opcode == OPCODE_SLLW_Y)
			aluinst = ALUINST_SLLW;
		else if (opcode == OPCODE_RRCW_Y || opcode == OPCODE_RRCW_SPREL)
			aluinst = ALUINST_RRCW;
		else if (opcode == OPCODE_RLCW_Y || opcode == OPCODE_RLCW_SPREL)
			aluinst = ALUINST_RLCW;
		else if (opcode == OPCODE_SRAW_Y)
			aluinst = ALUINST_SRAW;
		else if (opcode == OPCODE_SLLW_Y_XL)
			aluinst = ALUINST_SLLW1;
		else if (opcode == OPCODE_SEX_Y_XL)
			aluinst = ALUINST_SEX;
		else if (opcode == OPCODE_CLTZ_Y)
			aluinst = ALUINST_CLTZ;
		else if (opcode == OPCODE_NEGW_Y)
			aluinst = ALUINST_SUBW;
		else if (opcode == OPCODE_BOOLW_Y)
			aluinst = ALUINST_BOOLW;
		else if (opcode == OPCODE_XCH_YL_YH)
			aluinst = ALUINST_XCH0;
		else if (opcode == OPCODE_MUL_Y)
			aluinst = ALUINST_MUL;
		else if (opcode_is_mad(opcode))
			aluinst = ALUINST_MAD;
		else if (opcode == OPCODE_MSK_IY_XL_IMMD)
			aluinst = ALUINST_MSK;
		else if (opcode == OPCODE_CAX_IY_ZL_XL)
			aluinst = ALUINST_CAX;
		else if (opcode == OPCODE_CAXW_IY_Z_X)
			aluinst = ALUINST_CAXW;
		else if (opcode == OPCODE_LDW_Y_D)
			aluinst = ALUINST_SEX;
		else if (opcode == OPCODE_CPW_Y_IMMD) // Same operation as subw, but result is discarded.
			aluinst = ALUINST_SUBW;
		else if (opcode == OPCODE_XCH_XL_SPREL || opcode == OPCODE_XCH_XL_IY || opcode == OPCODE_XCH_F_SPREL || opcode == OPCODE_XCHW_X_IY || opcode == OPCODE_XCHW_Y_SPREL)
			aluinst = ALUINST_XCHW;
		else if (opcode == OPCODE_LDW_ISPREL_Y)
			aluinst = ALUINST_XCHW;
		else if (opcode_is_ldw_y(opcode) || opcode == OPCODE_CALL_IMMD || opcode == OPCODE_CALL_Y || opcode_is_tstw(opcode) || opcode_is_pushw(opcode) || opcode == OPCODE_POPW_Y ||
			opcode == OPCODE_LDW_DIR_Y || opcode == OPCODE_LDW_SPREL_Y || opcode == OPCODE_LDW_ZREL_Y || opcode == OPCODE_LDW_X_Y || opcode == OPCODE_LDW_Z_Y || opcode == OPCODE_LDW_IY_X || opcode == OPCODE_LDW_YREL_X)
			aluinst = ALUINST_PASSW0;
		else
			aluinst = ALUINST_PASS0;

		if (opcode == OPCODE_PUSH_IMMD)
			next_flags = flags;
		else if (opcode == OPCODE_XCH_F_SPREL)
			next_flags = result_reg[7:0];
		else
		begin
			// h flag
			if (opcode_is_sub(opcode) || opcode_is_sbc(opcode) || opcode_is_add(opcode) || opcode_is_adc(opcode) || opcode_is_cp(opcode) || opcode_is_inc(opcode) || opcode_is_dec(opcode))
				next_flags[0] = h_out;
			else
				next_flags[0] = flags[0];
			// c flag
			if (opcode_is_tst(opcode))
				next_flags[1] = 0;
			else if (opcode_is_tstw(opcode))
				next_flags[1] = 1;
			else if (opcode_is_sub(opcode) || opcode_is_sbc(opcode) || opcode_is_add(opcode) || opcode_is_adc(opcode) || opcode_is_cp(opcode) ||
				opcode_is_srl(opcode) || opcode_is_sll(opcode) || opcode_is_rrc(opcode) || opcode_is_rlc(opcode) || opcode == OPCODE_SRA_XL || opcode_is_inc(opcode) || opcode_is_dec(opcode) || opcode == OPCODE_DAA_XL ||
				opcode_is_16_2(opcode) || opcode_is_16_1(opcode) && !opcode_is_pushw(opcode) && !opcode_is_clrw(opcode) && opcode != OPCODE_BOOLW_Y && opcode != OPCODE_XCH_YL_YH ||
				opcode == OPCODE_ADDW_Y_D || opcode == OPCODE_CPW_Y_IMMD)
				next_flags[1] = c_out;
			else
				next_flags[1] = flags[1];
			// n flag
			if (opcode_is_8_2(opcode) || opcode_is_tst(opcode) || opcode_is_16_2(opcode) || opcode_is_16_1(opcode) && !opcode_is_pushw(opcode) && !opcode_is_clrw(opcode) && opcode != OPCODE_BOOLW_Y && opcode != OPCODE_XCH_YL_YH && opcode != OPCODE_INCNW_Y ||
				opcode_is_inc(opcode) || opcode_is_dec(opcode) ||
				opcode == OPCODE_ADDW_Y_D || opcode == OPCODE_CPW_Y_IMMD ||
				opcode == OPCODE_MUL_Y || opcode_is_mad(opcode) ||
				opcode == OPCODE_SEX_Y_XL)
				next_flags[2] = n_out;
			else
				next_flags[2] = flags[2];
			// z flag
			if (opcode_is_8_2(opcode) ||
				opcode_is_srl(opcode) || opcode_is_sll(opcode) || opcode_is_rrc(opcode) || opcode_is_rlc(opcode) ||  opcode == OPCODE_SRA_XL || opcode_is_inc(opcode) || opcode_is_dec(opcode) || opcode_is_tst(opcode) ||
				opcode_is_xchb(opcode) ||
				opcode == OPCODE_DAA_XL || opcode == OPCODE_BOOL_XL ||
				opcode_is_16_2(opcode) || opcode_is_16_1(opcode) && !opcode_is_pushw(opcode) && !opcode_is_clrw(opcode) && opcode != OPCODE_XCH_YL_YH ||
				opcode == OPCODE_ADDW_Y_D || opcode == OPCODE_CPW_Y_IMMD ||
				opcode == OPCODE_SLLW_Y_XL ||
				opcode == OPCODE_MUL_Y || opcode_is_mad(opcode) ||
				opcode == OPCODE_SEX_Y_XL || opcode == OPCODE_ZEX_Y_XL ||
				opcode == OPCODE_CAX_IY_ZL_XL || opcode == OPCODE_CAXW_IY_Z_X) 
				next_flags[3] = z_out;
			else
				next_flags[3] = flags[3];
			// o flag
			if (opcode_is_sub(opcode) || opcode_is_sbc(opcode) || opcode_is_add(opcode) || opcode_is_adc(opcode) || opcode_is_cp(opcode) ||
				opcode_is_inc(opcode) || opcode_is_dec(opcode) || opcode_is_tst(opcode) ||
				opcode_is_addw(opcode) || opcode_is_adcw(opcode) || opcode_is_subw(opcode) || opcode_is_sbcw(opcode) ||
				opcode_is_incw(opcode) && opcode != OPCODE_INCNW_Y || opcode == OPCODE_DECW_SPREL || opcode_is_adcw0(opcode) || opcode_is_sbcw0(opcode) || opcode == OPCODE_NEGW_Y || opcode_is_tstw(opcode))
				next_flags[4] = o_out;
			else
				next_flags[4] = flags[4];

			// hidden flags used internally
			if (opcode == OPCODE_NOP || opcode == OPCODE_ADDW_SP_D || opcode == OPCODE_RETI && next_opcode != OPCODE_RET)
				next_flags[7:5] = flags[7:5];
			else begin
				if (opcode == OPCODE_SWAPOP ||
					opcode == OPCODE_RET && !flags[5])
					next_flags[5] = 1;
				else
					next_flags[5] = 0;
				if (opcode == OPCODE_ALTACC1)
					next_flags[7:6] = 1;
				else if (opcode == OPCODE_ALTACC2)
					next_flags[7:6] = 2;
				else if (opcode == OPCODE_ALTACC3)
					next_flags[7:6] = 3;
				else
					next_flags[7:6] = 0;
			end
		end
	end

	assign regwrite_addr =
		(opcode_is_8_2(opcode) && !swapop_in || opcode_is_8_1_xl(opcode) || opcode_is_xchb(opcode) || opcode == OPCODE_XCH_XL_SPREL || opcode == OPCODE_XCH_XL_IY || opcode == OPCODE_ROT_XL_IMMD || opcode == OPCODE_POP_XL || opcode == OPCODE_THRD_XL || opcode_is_ld_xl(opcode) && !swapop_in) ?
			(accsel_in == ACCSEL_YL_Z ? 1 : accsel_in == ACCSEL_ZL_X ? 2 : 0) :
		(opcode_is_8_2_xh(opcode) && swapop_in) ? 0 :
		(opcode_is_8_2_zl(opcode) && swapop_in) ? 2 :
		((opcode_is_8_2_yl(opcode) || opcode_is_8_2_yh(opcode)) && swapop_in) ? 1 :
		(opcode == OPCODE_LD_XL_XH && swapop_in) ? 0:
		((opcode == OPCODE_LD_XL_YL || opcode == OPCODE_LD_XL_YH) && swapop_in) ? 1 :
		((opcode == OPCODE_LD_XL_ZL || opcode == OPCODE_LD_XL_ZH) && swapop_in) ? 2 :
		(opcode == OPCODE_LDW_X_Y) ?
			0 :
		(opcode == OPCODE_LDW_Z_Y || opcode == OPCODE_CAX_IY_ZL_XL || opcode == OPCODE_CAXW_IY_Z_X) ?
			2 :
		(opcode_is_16_2(opcode) || opcode_is_16_1_y(opcode) || opcode == OPCODE_SLLW_Y_XL || opcode == OPCODE_ADDW_Y_D || opcode == OPCODE_MUL_Y || opcode_is_ldw_y(opcode) || opcode == OPCODE_SEX_Y_XL || opcode == OPCODE_ZEX_Y_XL || opcode == OPCODE_POPW_Y || opcode == OPCODE_XCHW_Y_SPREL || opcode == OPCODE_DNJNZ_YH_D) ?
			(accsel_in == ACCSEL_ZL_X ? 0 : accsel_in == ACCSEL_YL_Z ? 2 : 1) :
		opcode_is_8_1_zh(opcode) ?
			2 :
		opcode_is_mad(opcode) || opcode == OPCODE_XCHW_X_IY ?
			0 :
		'x;
	assign regwrite_en =
		interrupt_start ? 0 :
		(opcode_is_8_2(opcode) && !opcode_is_cp(opcode) && !swapop_in || opcode_is_8_1(opcode) || opcode_is_xchb(opcode) || opcode == OPCODE_XCH_XL_SPREL || opcode == OPCODE_XCH_XL_IY || opcode == OPCODE_ROT_XL_IMMD || opcode == OPCODE_POP_XL || opcode == OPCODE_THRD_XL || opcode_is_ld_xl(opcode) && !swapop_in) ?
			(accsel_in == ACCSEL_XH_Y ? 2'b10 : 2'b01) :
		((opcode_is_8_2_xh(opcode) || opcode_is_8_2_yh(opcode)) && swapop_in) ? 2'b10 :
		((opcode_is_8_2_zl(opcode) || opcode_is_8_2_yl(opcode)) && swapop_in || opcode == OPCODE_CAX_IY_ZL_XL && !z_out) ? 2'b01 :
		((opcode == OPCODE_LD_XL_YL || opcode == OPCODE_LD_XL_ZL) && swapop_in) ? 2'b01 :
		((opcode == OPCODE_LD_XL_XH || opcode == OPCODE_LD_XL_YH || opcode == OPCODE_LD_XL_ZH) && swapop_in) ? 2'b10 :
		(opcode_is_16_2(opcode) || opcode_is_16_1_y(opcode) || opcode == OPCODE_SLLW_Y_XL || opcode == OPCODE_ADDW_Y_D || opcode == OPCODE_MUL_Y || opcode_is_mad(opcode) || opcode == OPCODE_CAXW_IY_Z_X && !z_out ||
		opcode_is_ldw_y(opcode) || opcode == OPCODE_LDW_X_Y || opcode == OPCODE_LDW_Z_Y || opcode == OPCODE_SEX_Y_XL || opcode == OPCODE_ZEX_Y_XL || opcode == OPCODE_POPW_Y || opcode == OPCODE_XCHW_X_IY || opcode == OPCODE_XCHW_Y_SPREL) ? 2'b11 :
		opcode == OPCODE_DNJNZ_YH_D ? 2'b10 :
		0;
	assign regwrite_data =
		((opcode_is_8_2(opcode) && !opcode_is_cp(opcode)) && accsel_in == ACCSEL_XH_Y || (opcode_is_8_2_xh(opcode) || opcode_is_8_2_yh(opcode)) && swapop_in) ? {result_reg[7:0], 8'bx} :
		((opcode == OPCODE_LD_XL_XH || opcode == OPCODE_LD_XL_YH || opcode == OPCODE_LD_XL_ZH) && swapop_in) ? {result_reg[7:0], 8'bx} :
		((opcode_is_8_1(opcode) || opcode_is_xchb(opcode) || opcode == OPCODE_ROT_XL_IMMD || opcode == OPCODE_POP_XL || opcode_is_ld_xl(opcode) && !swapop_in) && accsel_in == ACCSEL_XH_Y) ? {result_reg[7:0], 8'bx} :
		opcode == OPCODE_DNJNZ_YH_D ? {result_reg[7:0], 8'bx} :
		opcode == OPCODE_THRD_XL ? 0 : // todo: fix for multithreaded implementations
		result_reg;

	assign dwrite_addr =
		interrupt_start ? sp - 2 :
		(opcode == OPCODE_XCH_XL_IY || opcode == OPCODE_MSK_IY_XL_IMMD || opcode == OPCODE_LDW_IY_X || opcode == OPCODE_XCHW_X_IY) ? (accsel_in == ACCSEL_ZL_X ? x : accsel_in == ACCSEL_YL_Z ? z : y) :
		(opcode == OPCODE_LD_IY_XL || opcode == OPCODE_CAX_IY_ZL_XL || opcode == OPCODE_CAXW_IY_Z_X) ? y :
		opcode_is_push(opcode) ? sp - 1 :
		(opcode_is_8_1_dir(opcode) || opcode_is_xchb(opcode) || opcode_is_16_1_dir(opcode) || (opcode_is_8_2_dir(opcode) || opcode_is_16_2_dir(opcode)) && swapop_in || opcode == OPCODE_LD_DIR_XL || opcode == OPCODE_LDW_DIR_Y) ? inst[23:8] :
		(opcode_is_8_1_sprel(opcode) || opcode_is_16_1_sprel(opcode) || opcode == OPCODE_XCH_XL_SPREL || opcode == OPCODE_XCH_F_SPREL ||
		(opcode_is_8_2_sprel(opcode) || opcode_is_16_2_sprel(opcode)) && swapop_in || opcode == OPCODE_LD_SPREL_XL || opcode == OPCODE_LDW_SPREL_Y || opcode == OPCODE_XCHW_Y_SPREL) ? sp + inst[15:8] :
		(opcode_is_8_2_zrel(opcode) && swapop_in || opcode_is_16_1_zrel(opcode) || opcode == OPCODE_LD_ZREL_XL || opcode == OPCODE_MAD_X_ZREL_YL || opcode == OPCODE_LDW_ZREL_Y) ? z + inst[23:8] :
		opcode == OPCODE_LD_YREL_XL || opcode == OPCODE_LDW_YREL_X ? y + inst[15:8] : 
		opcode_is_pushw(opcode) ? sp - 2 :
		(opcode == OPCODE_CALL_IMMD || opcode == OPCODE_CALL_Y) ? sp - 2 :
		opcode == OPCODE_LDW_ISPREL_Y ? result_reg :
		'x;
	assign dwrite_en =
		reset ? 2'b00 :
		interrupt_start ? 2'b11 :
		(opcode_is_8_1_dir(opcode) || opcode_is_8_1_sprel(opcode) || opcode_is_push(opcode) || opcode_is_xchb(opcode) ||
			opcode == OPCODE_XCH_XL_SPREL || opcode == OPCODE_XCH_XL_IY || opcode == OPCODE_XCH_F_SPREL || opcode == OPCODE_MSK_IY_XL_IMMD || opcode == OPCODE_CAX_IY_ZL_XL && z_out ||
			opcode == OPCODE_LD_DIR_XL || opcode == OPCODE_LD_SPREL_XL || opcode == OPCODE_LD_ZREL_XL || opcode == OPCODE_LD_IY_XL || opcode == OPCODE_LD_YREL_XL || opcode == OPCODE_XCHW_X_IY ||
			(opcode_is_8_2_dir(opcode) || opcode_is_8_2_sprel(opcode) || opcode_is_8_2_zrel(opcode)) && !opcode_is_cp(opcode) && swapop_in) ? 2'b01 :
			opcode_is_pushw(opcode) || opcode == OPCODE_CALL_IMMD || opcode == OPCODE_CALL_Y ||
			opcode == OPCODE_LDW_DIR_Y || opcode == OPCODE_LDW_SPREL_Y || opcode == OPCODE_LDW_ZREL_Y || opcode == OPCODE_LDW_ISPREL_Y || opcode == OPCODE_LDW_IY_X || opcode == OPCODE_LDW_YREL_X ||
			((opcode_is_16_2_dir(opcode) || opcode_is_16_2_sprel(opcode)) && swapop_in) || opcode == OPCODE_CAXW_IY_Z_X && z_out||
			opcode_is_16_1_dir(opcode) || opcode_is_16_1_sprel(opcode) || opcode_is_16_1_zrel(opcode) || opcode == OPCODE_XCHW_Y_SPREL ? 2'b11 :
		2'b00;
	assign dwrite_data = dwrite_en ? result_mem : 'x;

	assign trap =
		!reset && (opcode == OPCODE_TRAP);

	always @(posedge clk)
	begin
		if(reset)
			flags = 8'b00000000;
		else
			flags = next_flags;
	end

	always @(negedge clk)
	begin
		$display("CPU negedge pc %h nextinst %h inst %h", pc, next_inst, inst);
	end
endmodule

`end_keywords

