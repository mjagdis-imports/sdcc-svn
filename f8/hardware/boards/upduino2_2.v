`begin_keywords "1800-2009"

`include "../clkdiv.v"
`include "../system_2.v"

// Test module for use on UPDuino v2.0 board.

// Default: 9 KB ROM, 6 KB RAM, 2 MHz system clock.
module icebreaker #(parameter ROMSIZE = 9216, RAMSIZE = 6144, CLKDIV = 6) (
	inout tri LED_RED_N, inout tri LED_GRN_N, inout tri LED_BLU_N);
	wire [7:0] gpio0pins, gpio1pins, gpio2pins;
	wire clk;
	wire trap;
	wire power_on_reset;

	SB_HFOSC #(.CLKHF_DIV("0b10")) inthosc(.CLKHFPU(1'b1),.CLKHFEN(1'b1),.CLKHF(CLK)); // Internal oscillator configured for 12 MHz.
 
	// Reset button
	assign power_on_reset = 0;

	clkdiv #(.CLKDIV(CLKDIV)) clkdiv(.*);
	system #(.ROMSIZE(ROMSIZE), .RAMSIZE(RAMSIZE)) system(.*);

	// RGB LED
	assign LED_RED_N = gpio0pins[0];
	assign gpio0pins[0] = LED_RED_N;
	assign LED_GRN_N = gpio0pins[1];
	assign gpio0pins[1] = LED_GRN_N;
	assign LED_BLU_N = gpio0pins[2];
	assign gpio0pins[2] = LED_BLU_N;
endmodule

`end_keywords

