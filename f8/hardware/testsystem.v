`include "system.v"

`begin_keywords "1800-2009"

// Test module for RTL Verilog design of f8.

`timescale 1us/1ns

module clkgen (clk);
	output reg clk;

	initial
		clk = 0;

	always #5 clk = !clk;
endmodule

module testsystem ();
	wire [7:0] gpio0pins;
	wire clk;
	wire trap;
	reg power_on_reset;

	initial
	begin
		$dumpfile("test.vcd");
    		$dumpvars(0,testsystem);
    		power_on_reset <= 1;
    		#20
    		power_on_reset <= 0;
		#2180
		$finish;
	end

	clkgen clkgen(.*);
	system system(.*);

	always @(posedge trap)
	begin
		$display("ERROR: TRAP");
		#20
		$finish;
	end
endmodule

`end_keywords

