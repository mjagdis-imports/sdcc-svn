`begin_keywords "1800-2009"

`include "cpu2.v"
`include "rom2.v"
`include "ram2.v"
`include "memory2.v"
`include "io2.v"

// SoC. trap output line needed for tests only.
module system  #(parameter ROMSIZE = 8192, RAMADDRBITS = 13, MEMADDRBASE = 16'h2000) (inout tri logic [7:0] gpio0pins, inout tri logic [7:0] gpio1pins, inout tri logic [7:0] gpio2pins,
	input logic clk, power_on_reset, output trap);

	logic [14:0] read_addr_even, read_addr_odd, write_addr_even, write_addr_odd;
	logic [7:0] read_data_even, read_data_odd, write_data_even, write_data_odd;
	logic write_en_even, write_en_odd;

	logic reset;
	logic interrupt;

	logic mem_write_en_even, mem_write_en_odd;
	logic [7:0] mem_read_data_even, mem_read_data_odd;

	logic io_write_en_even, io_write_en_odd;
	logic [7:0] io_read_data_even, io_read_data_odd;

	cpu cpu(.mem_write_addr_even(write_addr_even), .mem_write_addr_odd(write_addr_odd), .mem_read_addr_even(read_addr_even), .mem_read_addr_odd(read_addr_odd), .mem_write_en_even(write_en_even), .mem_write_en_odd(write_en_odd),
		.mem_write_data_even(write_data_even), .mem_write_data_odd(write_data_odd), .mem_read_data_even(read_data_even), .mem_read_data_odd(read_data_odd),
		.*);

	assign mem_write_en_even = (write_addr_even >= MEMADDRBASE / 2) ? write_en_even : 2'b00;
	assign mem_write_en_odd = (write_addr_odd >= MEMADDRBASE / 2) ? write_en_odd : 2'b00;
	memory #(.ROMSIZE(ROMSIZE), .RAMADDRBITS(RAMADDRBITS)) memory(.write_en_even(mem_write_en_even), .write_en_odd(mem_write_en_odd), .read_data_even(mem_read_data_even), .read_data_odd(mem_read_data_odd), .*);

	assign io_write_en_even = (write_addr_even < MEMADDRBASE / 2 ) ? write_en_even : 2'b00;
	assign io_write_en_odd = (write_addr_odd < MEMADDRBASE / 2) ? write_en_odd : 2'b00;
	iosystem iosystem(.write_en_even(io_write_en_even), .write_en_odd(io_write_en_odd), .read_data_even(io_read_data_even), .read_data_odd(io_read_data_odd) , .*);

	logic read_mem_even, read_mem_odd;
	always_ff @(posedge clk)
	begin
		read_mem_even <= read_addr_even >= MEMADDRBASE / 2;
		read_mem_odd <= read_addr_odd >= MEMADDRBASE / 2;
	end

	assign read_data_even = read_mem_even ? mem_read_data_even : io_read_data_even;
	assign read_data_odd = read_mem_odd ? mem_read_data_odd : io_read_data_odd;
endmodule

`end_keywords

