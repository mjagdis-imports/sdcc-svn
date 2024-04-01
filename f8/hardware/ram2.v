`begin_keywords "1800-2009"

`include "dualportram.v"

// ADDRBITS = 10: 1K RAM at 0x3c00 to 0x3fff
// ADDRBITS = 11: 2K RAM at 0x3800 to 0x3fff
// ADDRBITS = 12: 4K RAM at 0x3000 to 0x3fff
// ADDRBITS = 13: 8K RAM at 0x2000 to 0x3fff
module ram #(parameter ADDRBITS = 10)
	(input logic [14:0] dread_addr_even, output logic [7:0] dread_data_even, input logic [14:0] dwrite_addr_even, input logic [7:0] dwrite_data_even, input logic dwrite_en_even,
	input logic [14:0] dread_addr_odd, output logic [7:0] dread_data_odd, input logic [14:0] dwrite_addr_odd, input logic [7:0] dwrite_data_odd, input logic dwrite_en_odd,
	input logic clk);

	localparam SIZE = 1 << (ADDRBITS - 1);
	localparam logic [15:0] RAMBASE = 16'h4000 - SIZE;

	logic [ADDRBITS-1:0] dwrite_addr_rambased_even, dwrite_addr_rambased_odd;
	logic [15:0] dread_addr_rambased_even, dread_addr_rambased_odd;

	assign dwrite_addr_rambased_even = dwrite_addr_even - RAMBASE;
	assign dwrite_addr_rambased_odd = dwrite_addr_odd - RAMBASE;
	assign dread_addr_rambased_even = dread_addr_even - RAMBASE;
	assign dread_addr_rambased_odd = dread_addr_odd - RAMBASE;

	dualportram #(.ADDRBITS(ADDRBITS-1)) evenram
		(.din(dwrite_data_even), .write_en(dwrite_en_even), .waddr(dwrite_addr_even[ADDRBITS-1:1]),
		.raddr(dread_addr_even[ADDRBITS-1:1]), .dout(dread_data_even),
		.clk(clk));
	dualportram #(.ADDRBITS(ADDRBITS-1)) oddram
		(.din(dwrite_data_odd), .write_en(dwrite_en_odd), .waddr(dwrite_addr_odd[ADDRBITS-1:1]),
		.raddr(dread_addr_odd[ADDRBITS-1:1]), .dout (dread_data_odd),
		.clk(clk));
endmodule

`end_keywords

