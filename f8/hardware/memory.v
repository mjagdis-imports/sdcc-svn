`begin_keywords "1800-2009"

// Memory subsystem

// 512B ROM
module rom(iread_addr, iread_data, iread_valid, dread_addr, dread_data, clk);
	parameter ROMSIZE = 2048;
	parameter logic [15:0] ROMBASE = 16'h4000;

	input [15 : 0] iread_addr;
	output reg [23 : 0] iread_data;
	output reg iread_valid;
	input [15 : 0] dread_addr;
	output reg [15 : 0] dread_data;
	input clk;

	logic [15:0] dread_addr_rombased;

	(* ram_style = "block" *) reg [7:0] rom[ROMSIZE - 1 : 0];

	logic [15:0] iread_addr_rombased;

	assign iread_addr_rombased = iread_addr - ROMBASE;
	assign dread_addr_rombased = dread_addr - ROMBASE;

	initial $readmemh("test.vmem", rom);
		
	always @(posedge clk)
	begin
		iread_data[7 : 0] <= rom[iread_addr_rombased];
		iread_data[15 : 8] <= rom[iread_addr_rombased + 1];
		iread_data[23 : 16] <= rom[iread_addr_rombased + 2];
		dread_data[7:0] <= rom [dread_addr_rombased];
		dread_data[15:8] <= rom [dread_addr_rombased + 1];
		iread_valid <= 1;
	end
endmodule

module memory(iread_addr, iread_data, iread_valid, dread_addr, dread_data, dwrite_addr, dwrite_data, dwrite_en, clk);
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

	rom rom(.dread_data(dread_data_rom), .*);
	ram ram(.dread_data(dread_data_ram), .*);

	logic dread_rom;
	always @(posedge clk)
	begin
		dread_rom = dread_addr >= ROMBASE;
	end
	assign dread_data = dread_rom ? dread_data_rom : dread_data_ram;
endmodule

`end_keywords

