`begin_keywords "1800-2009"

`include "system_2.v"

// Test module for RTL Verilog design of f8.

`timescale 1us/1ns

module clkgen (output logic clk);
	initial
		clk = 0;

	always #2 clk = !clk;
endmodule

// 10 KB ROM, 8 KB RAM.
module testsystem #(parameter ROMSIZE = 10240, RAMSIZE = 8192, SIMULATIONTIME = 8200) ();
	logic [7:0] gpio0pins, gpio1pins, gpio2pins;
	logic clk;
	logic trap;
	logic power_on_reset;

	initial
	begin
		$dumpfile("test_2.vcd");
    		$dumpvars(0,testsystem);
    		power_on_reset <= 1;
    		#20
    		power_on_reset <= 0;
		#(SIMULATIONTIME - 20)
		$finish;
	end

	clkgen clkgen(.*);
	system #(.ROMSIZE(ROMSIZE), .RAMSIZE(RAMSIZE)) system(.*);

	always @(posedge clk)
		if(trap)
		begin
			$display("ERROR: TRAP");
			#20
			$finish;
		end
endmodule

`end_keywords
