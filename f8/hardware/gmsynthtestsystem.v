`include "system_gmsynth.v"

`begin_keywords "1800-2009"

// Test module for (partial post-synthesis) Verilog design of f8.
// ROM is not synthesized to allow testing of the rest with different programs

`timescale 1us/1ns

module clkgen (clk);
	output reg clk;

	initial
		clk = 0;

	always #2 clk = !clk;
endmodule

module testsystem ();
	wire [7:0] gpio0pins, gpio1pins, gpio2pins;
	wire clk;
	wire trap;
	reg power_on_reset;

	initial
	begin
		$dumpfile("fullpostsynthtest.vcd");
    		$dumpvars(0,testsystem);
    		power_on_reset <= 1;
    		#20
    		power_on_reset <= 0;
		#8180
		$finish;
	end

	clkgen clkgen(.*);
	system system(.*);

	always @(posedge clk)
		if(trap)
		begin
			$display("ERROR: TRAP");
			#20
			$finish;
		end
endmodule

`end_keywords

