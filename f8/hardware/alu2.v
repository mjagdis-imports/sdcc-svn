`begin_keywords "1800-2009"

function automatic carry4(logic [3:0] op0, op1, input logic c_in);
	logic [4:0] result;
	result = {1'b0, op0} + op1 + {4'h0, c_in};
	return result[4];
endfunction

function automatic carry7(logic [6:0] op0, op1, input logic c_in);
	logic [7:0] result;
	result = {1'b0, op0} + op1 + c_in;
	return result[7];
endfunction

function automatic carry15(logic [14:0] op0, op1, input logic c_in);
	logic [15:0] result;
	result = {1'b0, op0} + op1 + {15'b0, c_in};
	return result[15];
endfunction

function automatic logic [7:0] rot(logic [7:0] op, logic [2:0] count);
	logic [15:0] tmp = {op, op} << count;
	return tmp[15:8];
endfunction

function automatic logic [7:0] daaadjust(logic [7:0] op, logic c_in, logic h_in);
	logic[7:0] adjust = 8'h00;
	logic[3:0] bound = 4'ha;
	if (h_in || op[3:0] >= 4'ha)
	begin
		adjust += 8'h06;
		bound = 4'h9;
	end
	if (c_in || op[7:4] >= bound)
		adjust += 8'h60;
	return adjust;
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

typedef struct packed
{
	logic[15:0] result;
	logic o, z, n, c, h;
} addsub_result_t;

function automatic addsub_result_t addsub(logic [15:0] op0, logic [15:0] op1, logic c_in, logic w_in);
	addsub_result_t result;
	logic [16:0] wresult;
	wresult = {1'b0, op0} + {1'b0, op1} + {16'b0, c_in};
	result.result = wresult[15:0];
	if (!w_in)
	begin
		result.h = carry4 (op0, op1, c_in);
		result.c = wresult[8];
		result.n = wresult[7];
		result.z = !(|wresult[7:0]);
		result.o = result.c ^ carry7(op0, op1, c_in);
	end
	else
	begin
		result.h = 'x;
		result.c = wresult[16];
		result.n = wresult[15];
		result.z = !(|wresult[15:0]);
		result.o = result.c ^ carry15(op0, op1, c_in);
	end
	return(result);
endfunction;

function automatic logic[15:0] mad(logic [7:0] op0, logic [7:0] op1, logic [7:0] op2, logic c_in);
	logic [15:0] product = {8'h00, op0} * {8'h00, op1};
	logic [15:0] result = product + op2 + c_in;
	return(result);
endfunction;

`end_keywords

