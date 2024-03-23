`begin_keywords "1800-2009"

// Memory subsystem

module memory(iread_addr, iread_data, iread_valid, dread_addr, dread_data, dwrite_addr, dwrite_data, dwrite_en, clk);
	parameter RAMADDRBITS = 10;
	parameter ROMSIZE = 2048;
	parameter logic [15:0] ROMBASE = 16'h4000;

	input [15 : 0] iread_addr;
	output reg [23 : 0] iread_data;
	output reg iread_valid;
	input [15 : 0] dread_addr;
	output reg [15 : 0] dread_data;
	input [15 : 0] dwrite_addr;
	input [15 : 0] dwrite_data;
	input [1:0] dwrite_en;
	input clk;

	logic [15:0] dread_data_rom, dread_data_ram;

	rom #(.SIZE(ROMSIZE), .ROMBASE(ROMBASE)) rom(.dread_data(dread_data_rom), .*);
	ram #(.ADDRBITS(RAMADDRBITS)) ram(.dread_data(dread_data_ram), .*);

	logic dread_rom;
	always @(posedge clk)
	begin
		dread_rom = dread_addr >= ROMBASE;
	end
	assign dread_data = dread_rom ? dread_data_rom : dread_data_ram;
endmodule

`end_keywords

