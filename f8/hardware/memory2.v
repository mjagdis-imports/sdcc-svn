`begin_keywords "1800-2009"

// Memory subsystem
// ROMBASE needs to be a multiple of 2.
module memory #(parameter RAMSIZE = 1024, ROMSIZE = 2048, logic [15:0] ROMBASE = 16'h4000)
	(input logic [14:0] read_addr_even, output logic [7:0] read_data_even, input logic [14:0] write_addr_even, input logic [7:0] write_data_even, input logic write_en_even,
	input logic [14:0] read_addr_odd, output logic [7:0] read_data_odd, input logic [14:0] write_addr_odd, input logic [7:0] write_data_odd, input logic write_en_odd,
	input logic clk);

	logic [7:0] read_data_rom_even, read_data_rom_odd, read_data_ram_even, read_data_ram_odd;

	rom #(.SIZE(ROMSIZE), .ROMBASE(ROMBASE)) rom(.read_data_even(read_data_rom_even), .read_data_odd(read_data_rom_odd),.*);
	ram #(.SIZE(RAMSIZE)) ram(.read_data_even(read_data_ram_even), .read_data_odd(read_data_ram_odd), .*);

	// Assumes that ROMBASE is a multiple of 2, so we don't need to do this separately for odd and even addresses.
	logic read_rom;
	always_ff @(posedge clk)
	begin
		read_rom <= read_addr_odd >= ROMBASE / 2;
	end
	assign read_data_even = read_rom ? read_data_rom_even : read_data_ram_even;
	assign read_data_odd = read_rom ? read_data_rom_odd : read_data_ram_odd;
endmodule

`end_keywords

