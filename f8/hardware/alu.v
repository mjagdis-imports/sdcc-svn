typedef enum logic [4:0] {
	ALUINST_ADD,
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
	ALUINST_CLR,
	ALUINST_SUBW,
	ALUINST_SBCW,
	ALUINST_ADDW,
	ALUINST_ADCW,
	ALUINST_ORW,
	ALUINST_XCHB,
	ALUINST_SEX,
	ALUINST_ADSW,
	ALUINST_PASS0,
	ALUINST_PASSW0,
	ALUINST_XCHW}
	aluinst_t;

module alu(output logic [15:0] result_reg, result_mem, input logic [15:0] op0, op1, op2, input aluinst_t aluinst, input logic swapop_in, input logic c_in, output logic z_out,  output logic n_out, output logic c_out);
	wire [16:0] result;
	wire wideop;

	assign wideop =
		(aluinst == ALUINST_SUBW || aluinst == ALUINST_SBCW || aluinst == ALUINST_ADDW || aluinst == ALUINST_ADCW || aluinst == ALUINST_ORW ||
		aluinst == ALUINST_ADSW || aluinst == ALUINST_SEX || aluinst == ALUINST_PASSW0);

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
		aluinst == ALUINST_CLR ? 0 :
		aluinst == ALUINST_SUBW ? (swapop_in ? op1[15:0] + ~op0[15:0] : op0[15:0] + ~op1[15:0]) + 1 :
		aluinst == ALUINST_SBCW ? (swapop_in ? op1[15:0] + ~op0[15:0] : op0[15:0] + ~op1[15:0]) + c_in :
		aluinst == ALUINST_ADDW ? op0[15:0] + op1[15:0] + 0 :
		aluinst == ALUINST_ADCW ? op0[15:0] + op1[15:0] + c_in :
		aluinst == ALUINST_ORW ? op0[15:0] | op1[15:0] :
		aluinst == ALUINST_XCHB ? (op1[7:0] & ~(1 << op2[2:0]) | (op0[0] << op2[2:0])) :
		aluinst == ALUINST_SEX ? {{8{op0[7]}}, op0[7:0]} :
		aluinst == ALUINST_ADSW ? op0[15:0] + {{8{op1[7]}}, op1[7:0]} + 0 :
		aluinst == ALUINST_PASS0 ? op0[7:0] :
		aluinst == ALUINST_PASSW0 ? op0 :
		'x;

	assign result_reg =
		(aluinst == ALUINST_XCHW) ? op1 :
		(aluinst == ALUINST_XCHB) ? op1[op2[2:0]] :
		result[15:0];
	assign result_mem =
		(aluinst == ALUINST_XCHW) ? op0 :
		result[15:0];
	assign c_out = 
		(aluinst == ALUINST_SRL || aluinst == ALUINST_RRC) ? op0[0] :
		(aluinst == ALUINST_SLL || aluinst == ALUINST_RLC) ? op0[7] :
		wideop ? result[16] : result[8];
	assign z_out = !(wideop ? |result_reg[15:0] : |result_reg[7:0]);
	assign n_out = wideop ? result[15] : result[7];

	//always @(op2)
	//begin
	//	$display("ALU negedge op0 %h op1 %h op2 %h", op0, op1, op2);
	//end
endmodule

