typedef enum logic [5:0] {
	ALUINST_ADD,	// 0x00
	ALUINST_ADC,
	ALUINST_SUB,
	ALUINST_SBC,
	ALUINST_AND,
	ALUINST_OR,
	ALUINST_XOR,
	ALUINST_SRL,
	ALUINST_SLL,
	ALUINST_RRC,
	ALUINST_RLC,
	ALUINST_INC,
	ALUINST_DEC,
	ALUINST_BOOL,
	ALUINST_DAA,
	ALUINST_ROT,
	ALUINST_SUBW,
	ALUINST_SBCW,
	ALUINST_ADDW,	// 0x12
	ALUINST_ADCW,
	ALUINST_ORW,
	ALUINST_XCHB,
	ALUINST_CLRW,
	ALUINST_INCW,
	ALUINST_ADCW0,
	ALUINST_SBCW0,
	ALUINST_SLLW,
	ALUINST_SRLW,
	ALUINST_RRCW,
	ALUINST_RLCW,
	ALUINST_SRAW,
	ALUINST_BOOLW,
	ALUINST_MUL,
	ALUINST_MAD,
	ALUINST_CLTZ,	// 0x22
	ALUINST_SEX,
	ALUINST_MSK,
	ALUINST_CAX,
	ALUINST_CAXW,
	ALUINST_ADSW,
	ALUINST_PASS0,
	ALUINST_PASSW0,
	ALUINST_XCHW}
	aluinst_t;

function automatic carry4(logic [3:0] op0, op1, input logic c_in);
	logic [4:0] result;
	result = op0 + op1 + c_in;
	return result[4];
endfunction

function automatic carry7(logic [6:0] op0, op1, input logic c_in);
	logic [7:0] result;
	result = op0 + op1 + c_in;
	return result[7];
endfunction

function automatic carry15(logic [14:0] op0, op1, input logic c_in);
	logic [15:0] result;
	result = op0 + op1 + c_in;
	return result[15];
endfunction

function automatic logic [7:0] clz(logic [15:0] op);
	logic [7:0] count = 16;
	begin: loop
	for (int i = 0; i < 16; i++)
	begin
		if (op[15 - i])
		begin
			count = i;
			//break; // Icarus Verilog 11 doesn't support break. Icarus Verilog 12 does.
			disable loop;
		end
	end
	end: loop
	return count;
endfunction

function automatic logic [7:0] ctz(logic [15:0] op);
	logic [7:0] count = 16;
	begin: loop
	for (int i = 0; i < 16; i++)
	begin
		if (op[i])
		begin
			count = i;
			//break; // Icarus Verilog 11 doesn't support break. Icarus Verilog 12 does.
			disable loop;
		end
	end
	end: loop
	return count;
endfunction

function automatic logic [7:0] rot(logic [7:0] op, logic [2:0] count);
	logic [15:0] tmp = {op, op} << count;
	return tmp[15:8];
endfunction

function automatic logic [8:0] daa(logic [7:0] op, logic c_in, logic h_in);
	logic[7:0] adjust = 8'h00;
	logic[3:0] bound = 4'ha;
	if (h_in || op[3:0] >= 4'ha)
	begin
		adjust += 8'h06;
		bound = 4'h9;
	end
	if (c_in || op[7:4] >= bound)
		adjust += 8'h60;
	return op + adjust;
endfunction

module alu(output logic [15:0] result_reg, result_mem, input logic [15:0] op0, op1, op2, input aluinst_t aluinst, input logic swapop_in, input logic c_in, input logic h_in, output logic o_out, output logic z_out, output logic n_out, output logic c_out, output logic h_out);
	wire [16:0] result;
	wire wideop, arithop;
	wire logic overflow, halfcarry;

	assign wideop =
		(aluinst == ALUINST_SUBW || aluinst == ALUINST_SBCW || aluinst == ALUINST_ADDW || aluinst == ALUINST_ADCW || aluinst == ALUINST_ORW ||
		aluinst == ALUINST_INCW || aluinst == ALUINST_ADCW0 || aluinst == ALUINST_SBCW0 ||
		aluinst == ALUINST_BOOLW || aluinst == ALUINST_MUL || aluinst == ALUINST_MAD ||
		aluinst == ALUINST_ADSW || aluinst == ALUINST_SEX || aluinst == ALUINST_CAXW || aluinst == ALUINST_PASSW0);
	assign arithop = (
		aluinst == ALUINST_ADD || aluinst == ALUINST_ADC || aluinst == ALUINST_SUB || aluinst == ALUINST_SBC ||
		aluinst == ALUINST_INC || aluinst == ALUINST_DEC ||
		aluinst == ALUINST_INCW || aluinst == ALUINST_ADCW0 || aluinst == ALUINST_SBCW0 ||
		aluinst == ALUINST_SUBW || aluinst == ALUINST_SBCW || aluinst == ALUINST_ADDW || aluinst == ALUINST_ADCW || aluinst == ALUINST_ADSW);

	assign result =
		aluinst == ALUINST_ADD ? op0[7:0] + op1[7:0] + 0:
		aluinst == ALUINST_ADC ? op0[7:0] + op1[7:0] + c_in:
		aluinst == ALUINST_SUB ? (swapop_in ? op1[7:0] + 8'(~op0[7:0]) : op0[7:0] + 8'(~op1[7:0])) + 1 :
		aluinst == ALUINST_SBC ? (swapop_in ? op1[7:0] + 8'(~op0[7:0]) : op0[7:0] + 8'(~op1[7:0])) + c_in :
		aluinst == ALUINST_AND ? op0[7:0] & op1[7:0] :
		aluinst == ALUINST_OR ? op0[7:0] | op1[7:0] :
		aluinst == ALUINST_XOR ? op0[7:0] ^ op1[7:0] :
		aluinst == ALUINST_SRL ? {1'b0, op0[7:0]} >> 1 :
		aluinst == ALUINST_SLL ? {op0[6:0], 1'b0} :
		aluinst == ALUINST_RRC ? {c_in, op0[7:0]} >> 1 :
		aluinst == ALUINST_RLC ? {op0[6:0], c_in} :
		aluinst == ALUINST_INC ? op0[7:0] + 8'h01 :
		aluinst == ALUINST_DEC ? op0[7:0] + 8'hff :
		aluinst == ALUINST_BOOL ? |op0[7:0] :
		aluinst == ALUINST_DAA ? daa(op0, c_in, h_in) :
		aluinst == ALUINST_ROT ? rot(op0, op1) :
		aluinst == ALUINST_CLRW ? 0 :
		aluinst == ALUINST_INCW ? op0[15:0] + 8'h01 :
		aluinst == ALUINST_ADCW0 ? op0[15:0] + c_in :
		aluinst == ALUINST_SBCW0 ? op0[15:0] + 16'hffff + c_in :
		aluinst == ALUINST_SUBW ? (swapop_in ? op1[15:0] + ~op0[15:0] : op0[15:0] + ~op1[15:0]) + 1 :
		aluinst == ALUINST_SBCW ? (swapop_in ? op1[15:0] + ~op0[15:0] : op0[15:0] + ~op1[15:0]) + c_in :
		aluinst == ALUINST_ADDW ? op0[15:0] + op1[15:0] + 0 :
		aluinst == ALUINST_ADCW ? op0[15:0] + op1[15:0] + c_in :
		aluinst == ALUINST_ORW ? op0[15:0] | op1[15:0] :
		aluinst == ALUINST_XCHB ? (op1[7:0] & ~(1 << op2[2:0]) | (op0[0] << op2[2:0])) :
		aluinst == ALUINST_SRLW ? {1'b0, op0[15:0]} >> 1 :
		aluinst == ALUINST_SLLW ? {op0[14:0], 1'b0} :
		aluinst == ALUINST_RRCW ? {c_in, op0[15:0]} >> 1 :
		aluinst == ALUINST_RLCW ? {op0[14:0], c_in} :
		aluinst == ALUINST_SRAW ? {op0[15], op0[15:0]} >> 1 :
		aluinst == ALUINST_BOOLW ? |op0[15:0] :
		aluinst == ALUINST_MUL ? op1[7:0] * op2[7:0] :
		aluinst == ALUINST_MAD ? op1[7:0] * op2[7:0] + op0[7:0] :
		aluinst == ALUINST_CLTZ ? {ctz(op0), clz(op0)} :
		aluinst == ALUINST_SEX ? {{8{op0[7]}}, op0[7:0]} :
		aluinst == ALUINST_MSK ? (op0[7:0] & ~op1[7:0] | op2[7:0] & op1[7:0]) :
		aluinst == ALUINST_CAX ? (op0[7:0] == op2[7:0]) :
		aluinst == ALUINST_CAXW ? (op0 == op2) :
		aluinst == ALUINST_ADSW ? op0[15:0] + {{8{op1[7]}}, op1[7:0]} + 0 :
		aluinst == ALUINST_PASS0 ? op0[7:0] :
		aluinst == ALUINST_PASSW0 ? op0 :
		'x;

	assign overflow = 
		aluinst == ALUINST_ADD ? carry7 (op0[7:0], op1[7:0], 0) ^ c_out :
		aluinst == ALUINST_ADC ? carry7 (op0[7:0], op1[7:0], c_in) ^ c_out :
		aluinst == ALUINST_SUB ? carry7 (op0[7:0], ~op1[7:0], 0) ^ c_out :
		aluinst == ALUINST_SBC ? carry7 (op0[7:0], ~op1[7:0], c_in) ^ c_out :
		aluinst == ALUINST_INC ? carry7 (op0[7:0], 0, 1) ^ c_out :
		aluinst == ALUINST_INCW ? carry15 (op0[15:0], 0, 1) ^ c_out :
		aluinst == ALUINST_ADCW0 ? carry15 (op0[15:0], 0, c_in) ^ c_out :
		aluinst == ALUINST_SBCW0 ? carry15 (op0[15:0], 16'hffff, c_in) ^ c_out :
		aluinst == ALUINST_ADDW ? carry15 (op0[15:0], op1[15:0], 0) ^ c_out :
		aluinst == ALUINST_ADCW ? carry15 (op0[15:0], op1[15:0], c_in) ^ c_out :
		aluinst == ALUINST_SUBW ? carry15 (op0[15:0], ~op1[15:0], 1) ^ c_out :
		aluinst == ALUINST_SBCW ? carry15 (op0[15:0], ~op1[15:0], c_in) ^ c_out :
		'x;
	assign halfcarry =
		aluinst == ALUINST_ADD ? carry4 (op0[7:0], op1[7:0], 0) :
		aluinst == ALUINST_ADC ? carry4 (op0[7:0], op1[7:0], c_in) :
		aluinst == ALUINST_SUB ? carry4 (op0[7:0], ~op1[7:0], 0) :
		aluinst == ALUINST_SBC ? carry4 (op0[7:0], ~op1[7:0], c_in) :
		aluinst == ALUINST_INC ? carry4 (op0[7:0], 0, 1) :
		'x;

	assign result_reg =
		(aluinst == ALUINST_XCHW) ? op1 :
		(aluinst == ALUINST_XCHB) ? op1[op2[2:0]] :
		(aluinst == ALUINST_CAX || aluinst == ALUINST_CAXW) ? (z_out ? 'x : op0) :
		result[15:0];
	assign result_mem =
		(aluinst == ALUINST_XCHW) ? op0 :
		(aluinst == ALUINST_CAX || aluinst == ALUINST_CAXW) ? (z_out ? op1 : 'x) :
		result[15:0];

	assign c_out = 
		(aluinst == ALUINST_SRL || aluinst == ALUINST_RRC) ? op0[0] :
		(aluinst == ALUINST_SLL || aluinst == ALUINST_RLC) ? op0[7] :
		(aluinst == ALUINST_DAA) ? c_in | result[8] :
		(aluinst == ALUINST_SRLW || aluinst == ALUINST_RRCW || aluinst == ALUINST_SRAW) ? op0[0] :
		(aluinst == ALUINST_SLLW || aluinst == ALUINST_RLCW) ? op0[15] :
		wideop ? result[16] : result[8];
	assign n_out = wideop ? result[15] : result[7];
	assign z_out = (aluinst == ALUINST_CAX || aluinst == ALUINST_CAXW) ? result[0] :
		!(wideop ? |result_reg[15:0] : |result_reg[7:0]);
	assign o_out = arithop ? overflow :
		wideop ? ^result_reg[15:0] : ^result_reg[7:0];
	assign h_out = halfcarry;

	//always @(op2)
	//begin
	//	$display("ALU negedge op0 %h op1 %h op2 %h", op0, op1, op2);
	//end
endmodule

