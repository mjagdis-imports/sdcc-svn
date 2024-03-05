`include "cpu_synth.v"
`include "ram_synth.v"
`include "memory.v"
`include "io_synth.v"

`begin_keywords "1800-2009"

`timescale 1us/1ns

module system (inout tri logic [7:0] gpio0pins, input logic clk, power_on_reset);
	parameter MEMADDRBASE = 16'h2000;
	
	wire [15:0] iread_addr, dread_addr, dwrite_addr;
	wire [23:0] iread_data;
	logic [15:0] dread_data, dwrite_data;
	wire iread_valid;
	wire [1:0] dwrite_en;
	wire reset;
	wire interrupt;
	wire trap;

	wire [1:0] mem_dwrite_en, io_dwrite_en;
	wire [15:0] mem_dread_data, io_dread_data;

	cpu cpu(.*);
	memory memory(.dwrite_en(mem_dwrite_en), .dread_data(mem_dread_data), .*);
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

	always @(posedge trap)
	begin
		$display("ERROR: TRAP");
		#20
		$finish;
	end
endmodule

module clkgen (clk);
	output reg clk;

	initial
		clk = 0;

	always #5 clk = !clk;
endmodule

module testsystem ();
	wire [7:0] gpio0pins;
	wire clk;
	reg power_on_reset;

	initial
	begin
		$dumpfile("postsynthtest.vcd");
    		$dumpvars(0,testsystem);
    		power_on_reset <= 1;
    		#20
    		power_on_reset <= 0;
		#2180
		$finish;
	end

	clkgen clkgen(.*);
	system system(.*);
endmodule

`end_keywords

