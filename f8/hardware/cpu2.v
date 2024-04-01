`begin_keywords "1800-2009"

`include "opcode.v"

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
	return(opcode == OPCODE_JP_IMMD || opcode == OPCODE_LDW_Y_IMMD);
endfunction

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
	logic [7:0] opcode;
	logic [15:0] memop;
	logic [15:0] memop_addr;

	registers registers(.addr_in(regwrite_addr), .data_in(regwrite_data), .write_en(regwrite_en), .*);

	assign opcode = inst[7:0];

	logic loading_upper_inst, loading_operand, inst_upper_valid, execute;

	// Memory read
	always_comb
	begin
		logic [15:0] mem_read_addr;
		if (opcode == OPCODE_JP_IMMD || opcode == OPCODE_LDW_Y_IMMD)
			memop_addr = pc + 1;
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
		logic [15:0] next_pc_noint;
		if(!execute)
			next_pc_noint = pc;
		else if(opcode == OPCODE_JP_IMMD)
			next_pc_noint = memop;
		else if(opcode == OPCODE_JP_Y)
			next_pc_noint = y;
		else if(opcode == OPCODE_JR_D)
			next_pc_noint = signed'(pc) + signed'(inst[15:8]);
		else if(opcode == OPCODE_LDW_Y_IMMD)
			next_pc_noint = pc + 3;
		else
			next_pc_noint = pc + 1;
		next_pc = next_pc_noint;
	end

	// Instruction execution	
	assign trap = !reset && (opcode == OPCODE_TRAP);
	always_comb
	begin
		regwrite_data = 'x;
		regwrite_addr = 'x;
		regwrite_en = 2'b00;
		if(execute)
		begin
			if(opcode == OPCODE_LDW_Y_IMMD)
			begin
				regwrite_data = memop;
				regwrite_addr = 1;
				regwrite_en = 2'b11;
			end
		end
	end
	
	assign mem_write_en_even = 0;
	assign mem_write_en_odd = 0;
endmodule

`end_keywords

