`begin_keywords "1800-2009"

`include "dualportram.v"

// ADDRBITS = 10: 1K RAM at 0x3c00 to 0x3fff
// ADDRBITS = 11: 2K RAM at 0x3800 to 0x3fff
// ADDRBITS = 12: 4K RAM at 0x3000 to 0x3fff
// ADDRBITS = 13: 8K RAM at 0x2000 to 0x3fff
module ram #(parameter ADDRBITS = 10)
	(input logic [14:0] read_addr_even, output logic [7:0] read_data_even, input logic [14:0] write_addr_even, input logic [7:0] write_data_even, input logic write_en_even,
	input logic [14:0] read_addr_odd, output logic [7:0] read_data_odd, input logic [14:0] write_addr_odd, input logic [7:0] write_data_odd, input logic write_en_odd,
	input logic clk);

	localparam SIZE = 1 << (ADDRBITS - 1);
	localparam logic [15:0] RAMBASE = 16'h4000 - SIZE;

	logic [ADDRBITS-1:0] write_addr_rambased_even, write_addr_rambased_odd;
	logic [15:0] read_addr_rambased_even, read_addr_rambased_odd;

	assign write_addr_rambased_even = write_addr_even - RAMBASE;
	assign write_addr_rambased_odd = write_addr_odd - RAMBASE;
	assign read_addr_rambased_even = read_addr_even - RAMBASE;
	assign read_addr_rambased_odd = read_addr_odd - RAMBASE;

	dualportram #(.ADDRBITS(ADDRBITS-1)) evenram
		(.din(write_data_even), .write_en(write_en_even), .waddr(write_addr_even[ADDRBITS-1:1]),
		.raddr(read_addr_even[ADDRBITS-1:1]), .dout(read_data_even),
		.clk(clk));
	dualportram #(.ADDRBITS(ADDRBITS-1)) oddram
		(.din(write_data_odd), .write_en(write_en_odd), .waddr(write_addr_odd[ADDRBITS-1:1]),
		.raddr(read_addr_odd[ADDRBITS-1:1]), .dout (read_data_odd),
		.clk(clk));
endmodule

`end_keywords

