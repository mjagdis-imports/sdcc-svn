`begin_keywords "1800-2009"

`include "opcode.v"

// Could be written nicer using unpacked structs, but icarus does not yet support those.
module registers(output logic [15:0] x, y, z, input logic [1:0] addr_in, input logic [15:0] data_in, input logic [1:0] write_en,
	output logic [7:0] f, input logic [7:0] next_f,
	output logic [15:0] sp, input logic [15:0] next_sp,
	output logic [15:0] pc, input logic [15:0] next_pc,
	output logic [23:0] inst, input logic [23:0] next_inst,
	output logic inst_upper_valid, input inst_upper_valid_next,
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
			pc = 16'h4000;
		else
			pc = next_pc;
		sp = next_sp;
		inst = next_inst;
		inst_upper_valid = inst_upper_valid_next;
	end
endmodule

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
	logic [23:0] inst, next_inst;
	logic inst_upper_valid, inst_upper_valid_next;

	logic mem_read_data;
	logic [7:0] opcode;

	registers registers(.addr_in(regwrite_addr), .data_in(regwrite_data), .write_en(regwrite_en), .*);

	assign opcode = inst[7:0];

	// Memory read
	always_comb
	begin
		logic [15:0] mem_read_addr;
		//if (inst_upper_valid || opcode == OPCODE_NOP)
		begin
			mem_read_addr = next_pc;
		end
		//else
		//begin
		//	mem_read_addr = pc + 1;
		//end
		if (mem_read_addr[0])
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

	always_comb
	begin
		//next_inst = ;
	end

	// Program counter
	always_comb
	begin
		logic [15:0] next_pc_noint;
		if (opcode == OPCODE_NOP)
			next_pc_noint = pc + 1;
		else if (opcode == OPCODE_JR_D)
			next_pc_noint = signed'(pc) + signed'(inst[15:8]);
		else
			next_pc_noint = pc;
		next_pc = next_pc_noint;
	end

	assign trap = 0;
	assign mem_write_en_even = 0;
	assign mem_write_en_odd = 0;
endmodule

`end_keywords

