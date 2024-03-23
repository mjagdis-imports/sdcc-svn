`include "cpu.v"
`include "rom.v"
`include "ram.v"
`include "memory.v"
`include "io.v"

`begin_keywords "1800-2009"

// SoC. trap output line needed for tests only.
module system (inout tri logic [7:0] gpio0pins, inout tri logic [7:0] gpio1pins, inout tri logic [7:0] gpio2pins,
	input logic clk, power_on_reset, output trap);
	parameter ROMSIZE = 8192;
	parameter RAMADDRBITS = 13;
	parameter MEMADDRBASE = 16'h2000;
	
	wire [15:0] iread_addr, dread_addr, dwrite_addr;
	wire [23:0] iread_data;
	logic [15:0] dread_data, dwrite_data;
	wire iread_valid;
	wire [1:0] dwrite_en;
	wire reset;
	wire interrupt;

	wire [1:0] mem_dwrite_en, io_dwrite_en;
	wire [15:0] mem_dread_data, io_dread_data;

	cpu cpu(.*);
	memory #(.ROMSIZE(ROMSIZE), .RAMADDRBITS(RAMADDRBITS)) memory(.dwrite_en(mem_dwrite_en), .dread_data(mem_dread_data), .*);
	iosystem iosystem(.dwrite_en(io_dwrite_en), .dread_data(io_dread_data), .*);

	logic [15:0] old_dread_addr;
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
	end
endmodule

`end_keywords

