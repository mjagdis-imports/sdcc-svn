/*-------------------------------------------------------------------------
  system2.v - SoC using multi-cycle f8 core

  Copyright (c) 2024, Philipp Klaus Krause philipp@colecovision.eu)

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2, or (at your option) any
  later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
-------------------------------------------------------------------------*/

`begin_keywords "1800-2009"

`include "cpu2.v"
`include "rom2.v"
`include "ram2.v"
`include "memory2.v"
`include "io2.v"

// SoC. trap output line needed for tests only.
// MEMADDRBASE needs to be a multiple of 2.
// Default 9 KB ROM, 6 KB RAM.
module system  #(parameter ROMSIZE = 9216, RAMSIZE = 6144, MEMADDRBASE = 16'h2000) (inout tri logic [7:0] gpio0pins, inout tri logic [7:0] gpio1pins, inout tri logic [7:0] gpio2pins,
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

	// Assumes that MEMADDRBASE is a multiple of 2, so we don't need to do this separately for odd and even addresses.
        logic write_addr_in_io_space;
        logic read_addr_in_io_space;
        assign write_addr_in_io_space = write_addr_odd < MEMADDRBASE / 2;
        assign read_addr_in_io_space = read_addr_odd < MEMADDRBASE / 2;

	assign mem_write_en_even = !write_addr_in_io_space ? write_en_even : 0;
	assign mem_write_en_odd = !write_addr_in_io_space ? write_en_odd : 0;
	memory #(.ROMSIZE(ROMSIZE), .RAMSIZE(RAMSIZE)) memory(.write_en_even(mem_write_en_even), .write_en_odd(mem_write_en_odd), .read_data_even(mem_read_data_even), .read_data_odd(mem_read_data_odd), .*);

	assign io_write_en_even = write_addr_in_io_space ? write_en_even : 0;
	assign io_write_en_odd = write_addr_in_io_space ? write_en_odd : 0;
	iosystem iosystem(.write_en_even(io_write_en_even), .write_en_odd(io_write_en_odd), .read_data_even(io_read_data_even), .read_data_odd(io_read_data_odd) , .*);

	logic read_io;
	always_ff @(posedge clk)
	begin
		read_io = read_addr_in_io_space;
	end

	assign read_data_even = read_io ? io_read_data_even : mem_read_data_even;
	assign read_data_odd = read_io ? io_read_data_odd : mem_read_data_odd;
endmodule

`end_keywords

