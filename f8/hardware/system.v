/*-------------------------------------------------------------------------
  system.v - SoC using single-cycle f8 core

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

`include "cpu.v"
`include "rom.v"
`include "ram.v"
`include "memory.v"
`include "io.v"

// SoC. trap output line needed for tests only.
module system  #(parameter ROMSIZE = 8192, RAMSIZE = 8192, MEMADDRBASE = 16'h2000) (inout tri logic [7:0] gpio0pins, inout tri logic [7:0] gpio1pins, inout tri logic [7:0] gpio2pins,
	input logic clk, power_on_reset, output trap);

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
	memory #(.ROMSIZE(ROMSIZE), .RAMSIZE(RAMSIZE)) memory(.dwrite_en(mem_dwrite_en), .dread_data(mem_dread_data), .*);
	iosystem iosystem(.dwrite_en(io_dwrite_en), .dread_data(io_dread_data), .*);

	logic [15:0] old_dread_addr;
	always_ff @(posedge clk)
	begin
		old_dread_addr <= dread_addr;
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

