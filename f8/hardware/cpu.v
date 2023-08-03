`begin_keywords "1800-2009"

`include "alu.v"
`include "opcode.v"

module regfile(output logic [15:0] x, y, z, sp, output logic [15:0] next_x, next_y, next_z, next_sp, input logic [1:0] addr_in, input logic [15:0] data_in, input logic [1:0] write_en, input logic clk);
	reg [15:0] regs[4];

	// Just some values suitable for a few tests I'm running - remove this later!
	initial
	begin
		regs[0] = 16'h0201;
		regs[1] = 16'h0100;
		regs[2] = 16'haa55;
		regs[3] = 16'haa55;
	end

	assign x = regs[0];
	assign y = regs[1];
	assign z = regs[2];
	assign sp = regs[3];

	assign next_x = {(addr_in == 0 && write_en[1]) ? data_in[15:8]: regs[0][15:8], (addr_in == 0 && write_en[0]) ? data_in[7:0]: regs[0][7:0]};
	assign next_y = {(addr_in == 1 && write_en[1]) ? data_in[15:8]: regs[1][15:8], (addr_in == 1 && write_en[0]) ? data_in[7:0]: regs[1][7:0]};
	assign next_z = {(addr_in == 2 && write_en[1]) ? data_in[15:8]: regs[2][15:8], (addr_in == 2 && write_en[0]) ? data_in[7:0]: regs[2][7:0]};
	assign next_sp = {(addr_in == 3 && write_en[1]) ? data_in[15:8]: regs[3][15:8], (addr_in == 3 && write_en[0]) ? data_in[7:0]: regs[3][7:0]};

	always @(posedge clk)
	begin
		if (write_en[0])
			regs[addr_in][7:0] = data_in[7:0];
		if (write_en[1])
			regs[addr_in][15:8] = data_in[15:8];
	end
endmodule

`define OP_LD 4'b0001 // 8-bit load
`define OP_LDW 4'b0010 // 16-bit load
`define OP_2 2'b10 // 8-bit two-operand instruction: 10OOOMMM
`define OP_ADD 3'b000
`define OP_ADC 3'b001
`define OP_SUB 3'b010
`define OP_SBC 3'b011
`define OP_CP 3'b100
`define OP_AND 3'b101
`define OP_OR 3'b110
`define OP_XOR 3'b111
`define OP_MODE_8_XH 3'b000
`define OP_MODE_8_YL 3'b001
`define OP_MODE_8_YH 3'b010
`define OP_MODE_8_ZL 3'b011
`define OP_MODE_8_IMMD 3'b100
`define OP_MODE_8_DIR 3'b101
`define OP_MODE_8_ZREL 3'b110
`define OP_MODE_8_SPREL 3'b111


module cpu(iread_addr, iread_data, ivalid, dread_addr, dread_data, dwrite_addr, dwrite_data, dwrite_en, clk, reset, trap);
	output [15 : 0] iread_addr;
	input [23 : 0] iread_data;
	input ivalid;
	output logic [15:0] dread_addr;
	input [15 : 0] dread_data;
	output [15 : 0] dwrite_addr;
	output [15 : 0] dwrite_data;
	output dwrite_en;
	input clk, reset;
	output trap;

	wire [23 : 0] next_inst;
	wire [7:0] next_opcode;
	wire nextinst_valid;
	reg [23 : 0] inst;
	wire [7:0] opcode;

	logic [15:0] op0, op1, op2;
	wire [15 : 0] result_reg, result_mem;
	aluinst_t aluinst;
	wire c_in, c_out, z_out, n_out;
	reg [7:0] flags;

	alu alu(.*);

	wire [15:0] x, y, z, sp;
	wire [15 : 0] next_x, next_y, next_z, next_sp;
	logic [7:0] next_flags;
	wire [1 : 0] regwrite_en;
	wire [1 : 0] regwrite_addr;

	logic [15:0] next_pc;
	logic [15:0] pc;

	regfile regfile(.addr_in(regwrite_addr), .data_in(result_mem), .write_en(regwrite_en), .*);

	assign next_opcode = next_inst[7:0];
	assign opcode = inst[7:0];

	// Handle program counter
	always @(posedge clk)
		pc <= next_pc;
	always @(posedge clk)
		inst <= next_inst;

	always_comb
	begin
		if(reset)
			next_pc = 16'h4000;
		else if(next_opcode == OPCODE_JP_IMMD)
			next_pc = next_inst[31:8];
		else if(next_opcode == OPCODE_JP_Y)
			next_pc = next_y;
		else if(next_opcode == OPCODE_JR_D ||
			next_opcode == OPCODE_JRZ_D && next_flags[3] || next_opcode == OPCODE_JRNZ_D && !next_flags[3] ||
			next_opcode == OPCODE_JRC_D && next_flags[1] || next_opcode == OPCODE_JRNC_D && !next_flags[1] ||
			next_opcode == OPCODE_JRN_D && next_flags[2] || next_opcode == OPCODE_JRNN_D && !next_flags[2])
			next_pc = signed'(pc) + signed'(next_inst[15:8]);
		else if(opcode_is_8_immd(next_opcode) || opcode_is_sprel(next_opcode) || opcode_is_jr_d(next_opcode) || next_opcode == OPCODE_LDW_Y_D)
			next_pc = pc + 2;
		else if(opcode_is_16_immd(next_opcode) || opcode_is_dir(next_opcode) || opcode_is_zrel(next_opcode))
			next_pc = pc + 3;
		else
			next_pc = pc + 1;
	end

	assign iread_addr = next_pc;
	assign next_inst = iread_data;
	assign c_in = 0;

	// Handle memory reads
	always_comb
	begin
		if(opcode_is_dir(opcode))
			dread_addr = next_inst[23:8];
		else if(opcode_is_sprel(opcode))
			dread_addr = next_sp + next_inst[15:8];
		else if(opcode_is_zrel(opcode))
			dread_addr = next_z + next_inst[23:8];
		else if (opcode == OPCODE_LD_XL_IY)
			dread_addr = next_y;
		else
			dread_addr = 'x;
	end

	assign c_in = flags[1];

	always_comb
	begin
		if(opcode_is_8_2(opcode))
			op0 = {8'bx, x[7:0]};
		else if(opcode_is_16_2(opcode))
			op0 = x;
		else if(opcode == OPCODE_LD_XL_IMMD || opcode == OPCODE_LDW_Y_D)
			op0 = {8'bx, inst[15:8]};
		else
			op0 = 'x;

		if(opcode_is_8_2_immd(opcode))
			op1 = {8'bx, inst[15:8]};
		else if(opcode_is_8_2_dir(opcode) || opcode_is_8_2_sprel(opcode) || opcode_is_8_2_zrel(opcode))
			op1 = {8'bx, dread_data[7 : 0]};
		else if(opcode_is_8_2_zl(opcode))
			op1 = z[7:0];
		else if(opcode_is_8_2_xh(opcode))
			op1 = x[15:8];
		else if(opcode_is_8_2_yl(opcode))
			op1 = y[7:0];
		else if(opcode_is_8_2_yh(opcode))
			op1 = y[15:8];
		else
			op1 = 'x;

		if (opcode_is_sub(opcode))
			aluinst = ALUINST_SUB;
		else if (opcode_is_sbc(opcode))
			aluinst = ALUINST_SBC;
		else if (opcode_is_add(opcode))
			aluinst = ALUINST_ADD;
		else if (opcode_is_adc(opcode))
			aluinst = ALUINST_ADC;
		else if (opcode_is_cp(opcode)) // Same operation as sub, but result is discarded.
			aluinst = ALUINST_SUB;
		else if (opcode_is_srl(opcode))
			aluinst = ALUINST_SRL;
		else if (opcode == OPCODE_LDW_Y_D)
			aluinst = ALUINST_SEX;
		/* todo: more instructions here*/
		else
			aluinst = ALUINST_PASS;

		if (0)
			;
		else
			next_flags[0] = flags[0];
		if (opcode_is_sub(opcode) || opcode_is_sbc(opcode) || opcode_is_add(opcode) || opcode_is_adc(opcode) || opcode_is_cp(opcode))
			next_flags[1] = c_out;
		else
			next_flags[1] = flags[1];
		if (opcode_is_8_2(opcode))
			next_flags[2] = n_out;
		else
			next_flags[2] = flags[2];
		if (opcode_is_8_2(opcode))
			next_flags[3] = z_out;
		else
			next_flags[3] = flags[3];
		if (0)
			;
		else
			next_flags[4] = flags[4];
	end

	assign regwrite_addr =
		(opcode_is_8_2(opcode) || opcode == OPCODE_LD_XL_IMMD) ? 0 :
		(opcode == OPCODE_LDW_Y_IMMD || opcode == OPCODE_LDW_Y_D) ? 1 :
		opcode == OPCODE_LDW_SP_Y ? 3 :
		'x;

	assign regwrite_en =
		(opcode_is_8_2(opcode) && !opcode_is_cp(opcode) || opcode == OPCODE_LD_XL_IMMD) ? 2'b01 :
		(opcode_is_16_2(opcode) || opcode == OPCODE_LDW_Y_IMMD || opcode == OPCODE_LDW_Y_D) ? 2'b11 :
		0;

	assign dwrite_addr =
		inst[7 : 0] == OPCODE_LD_IY_XL ? y :
		'x;
	assign dwrite_en =
		reset ? 2'b00 :
		(opcode == OPCODE_LD_IY_XL);
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

