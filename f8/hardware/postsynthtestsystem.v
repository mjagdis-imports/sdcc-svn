`include "cpu_synth.v"
`include "memory.v"
`include "io_synth.v"

`begin_keywords "1800-2009"

`timescale 1us/1ns

module clkgen (clk);
	output reg clk;

	initial
		clk = 0;

	always #5 clk = !clk;
endmodule

module testsystem ();
	parameter RAMADDRBASE = 16'h2000;
	
	wire [15:0] iread_addr, dread_addr, dwrite_addr;
	wire [23:0] iread_data;
	logic [15:0] dread_data, dwrite_data;
	wire iread_valid;
	wire [1:0] dwrite_en;
	wire clk;
	reg power_on_reset;
	wire reset;
	wire interrupt;
	wire trap;

	wire [1:0] mem_dwrite_en, io_dwrite_en;
	wire [15:0] mem_dread_data, io_dread_data;

	clkgen clkgen(.*);
	cpu cpu(.*);
	memory memory(.dwrite_en(mem_dwrite_en), .dread_data(mem_dread_data), .*);
	iosystem iosystem(.dwrite_en(io_dwrite_en), .dread_data(io_dread_data), .*);

	logic [15:0] old_dread_addr;
	always @(posedge clk)
	begin
		old_dread_addr = dread_addr;
	end

	assign mem_dwrite_en = (dwrite_addr >= RAMADDRBASE) ? dwrite_en : 2'b00;
	assign io_dwrite_en = (dwrite_addr < RAMADDRBASE) ? dwrite_en : 2'b00;

	always_comb
	begin
		if (old_dread_addr >= RAMADDRBASE)
			dread_data = mem_dread_data;
		else
			dread_data = io_dread_data;
	end

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

	always @(posedge trap)
	begin
		$display("ERROR: TRAP");
		#20
		$finish;
	end
endmodule

`end_keywords

