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
	ALUINST_SUBW,
	ALUINST_SEX,
	ALUINST_PASS0,
	ALUINST_PASS1,
	ALUINST_PASSW0,
	ALUINST_PASSW1}
	aluinst_t;

module alu(output logic [15:0] result_reg, result_mem, input logic [15:0] op0, op1, op2, input aluinst_t aluinst, input logic c_in, output logic z_out,  output logic n_out, output logic c_out);
	wire [16:0] result;
	wire wideop;

	assign wideop = (aluinst == ALUINST_SUBW || aluinst == ALUINST_SEX || aluinst == ALUINST_PASSW0 || aluinst == ALUINST_PASSW1);

	assign result =
		aluinst == ALUINST_ADD ? op0[7:0] + op1[7:0] :
		aluinst == ALUINST_ADC ? op0[7:0] + op1[7:0] + c_in:
		aluinst == ALUINST_SUB ? op0[7:0] + ~op1[7:0] + 1 :
		aluinst == ALUINST_SBC ? op0[7:0] + ~op1[7:0] + c_in :
		aluinst == ALUINST_AND ? op0[7:0] & op1[7:0] :
		aluinst == ALUINST_OR ? op0[7:0] | op1[7:0] :
		aluinst == ALUINST_XOR ? op0[7:0] ^ op1[7:0] :
		aluinst == ALUINST_SRL ? {1'b0, op0[7:0]} >> 1 :
		aluinst == ALUINST_SLL ? {op0[7:0], 1'b0} << 1 :
		aluinst == ALUINST_RRC ? {c_in, op0[7:0]} >> 1 :
		aluinst == ALUINST_RLC ? {op0[7:0], c_in} << 1 :
		aluinst == ALUINST_INC ? op0[7:0] + 8'h01 :
		aluinst == ALUINST_DEC ? op0[7:0] + 8'hff :
		aluinst == ALUINST_SUBW ? op0[15:0] + ~op1[15:0] + 1 :
		aluinst == ALUINST_SEX ? signed'(op0[7:0]) :
		aluinst == ALUINST_PASS0 ? op0[7:0] :
		aluinst == ALUINST_PASS1 ? op1[7:0] :
		aluinst == ALUINST_PASSW0 ? op0 :
		aluinst == ALUINST_PASSW1 ? op1 :
		'x;

	// For now just a pass-through dummy.
	assign result_reg = result[15:0];
	assign result_mem = result[15:0];
	assign c_out = 
		(aluinst == ALUINST_SRL || aluinst == ALUINST_RRC) ? op0[0] :
		(aluinst == ALUINST_SLL || aluinst == ALUINST_RLC) ? op0[7] :
		wideop ? result[16] : result[8];
	assign z_out = !(wideop ? |result[15:0] : |result[7:0]);
	assign n_out = wideop ? result[15] : result[7];

	//always @(op2)
	//begin
	//	$display("ALU negedge op0 %h op1 %h op2 %h", op0, op1, op2);
	//end
endmodule

