`include "rom2.v"
`include "ram2.v"
`include "memory2.v"

`begin_keywords "1800-2009"

// SoC. trap output line needed for tests only.
module system  #(parameter ROMSIZE = 8192, RAMADDRBITS = 13, MEMADDRBASE = 16'h2000) (inout tri logic [7:0] gpio0pins, inout tri logic [7:0] gpio1pins, inout tri logic [7:0] gpio2pins,
	input logic clk, power_on_reset, output trap);

	logic [14:0] dread_addr_even, dread_addr_odd, dwrite_addr_even, dwrite_addr_odd;
	logic [7:0] dread_data_even, dread_data_odd, dwrite_data_even, dwrite_data_odd;
	logic dwrite_en_even, dwrite_en_odd;

	logic reset;
	logic interrupt;

	logic mem_dwrite_en_even, mem_dwrite_en_odd;
	logic [7:0] mem_dread_data_even, mem_dread_data_odd;

	//cpu cpu(.*);
	memory #(.ROMSIZE(ROMSIZE), .RAMADDRBITS(RAMADDRBITS)) memory(.dwrite_en_even(mem_dwrite_en_even), .dwrite_en_odd(mem_dwrite_en_odd), .dread_data_even(mem_dread_data_even), .dread_data_odd(mem_dread_data_odd), .*);
	//iosystem iosystem(.dwrite_en(io_dwrite_en), .dread_data(io_dread_data), .*);

	/*logic [15:0] old_dread_addr;
	always @(posedge clk)
	begin
		old_dread_addr = dread_addr;
	end

	assign mem_dwrite_en = (dwrite_addr >= MEMADDRBASE) ? dwrite_en : 2'b00;
	assign io_dwrite_en = (dwrite_addr < MEMADDRBASE) ? dwrite_en : 2'b00;

	always_comb
	begin
		if (old_dread_addr >= MEMADDRBASE)
			dread_data = mem_dread_data;
		else
			dread_data = io_dread_data;
	end*/
endmodule

`end_keywords

