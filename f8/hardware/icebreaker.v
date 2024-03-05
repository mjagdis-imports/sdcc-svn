`include "system.v"

`begin_keywords "1800-2009"

// Test module for use on iCEBreaker FPGA board.

// Divide clock by 2.
module clkgen (output clk, input CLK);
	reg clk2;
	always @(posedge CLK)
	begin
		clk2 = !clk2;
	end
	always @(posedge clk2)
	begin
		clk = !clk;
	end
endmodule

module icebreaker (input logic CLK,
	inout tri PMOD_2_1, inout tri PMOD_2_2, inout tri PMOD_2_3, inout tri PMOD_2_4, inout tri PMOD_2_7, inout tri PMOD_2_8, inout tri PMOD_2_9, inout tri PMOD_2_10,
	input logic BTN_N);
	wire [7:0] gpio0pins;
	wire clk;
	wire trap;
	wire power_on_reset;

	assign power_on_reset = !BTN_N;
	assign PMOD_2_7 = gpio0pins[0];
	assign PMOD_2_1 = gpio0pins[1];
	assign PMOD_2_3 = gpio0pins[2];
	assign PMOD_2_2 = gpio0pins[3];
	assign PMOD_2_8 = gpio0pins[4];
	assign PMOD_2_9 = gpio0pins[5];
	assign PMOD_2_4 = gpio0pins[6];
	assign PMOD_2_10 = gpio0pins[7];
	
	clkgen clkgen(.*);
	system system(.*);
endmodule

`end_keywords

