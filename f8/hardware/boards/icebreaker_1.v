`begin_keywords "1800-2009"

`include "../clkdiv.v"
`include "../system_1.v"

// Test module for use on iCEBreaker FPGA board.

// Default: 2 KB ROM, 1 KB RAM, 2 MHz system clock.
module icebreaker #(parameter ROMSIZE = 2048, RAMSIZE = 1024, CLKDIV = 6) (input logic CLK,
	inout tri PMOD_2_1, inout tri PMOD_2_2, inout tri PMOD_2_3, inout tri PMOD_2_4, inout tri PMOD_2_7, inout tri PMOD_2_8, inout tri PMOD_2_9, inout tri PMOD_2_10,
	inout tri PMOD_1A_1, inout tri PMOD_1A_2, inout tri PMOD_1A_3, inout tri PMOD_1A_4, inout tri PMOD_1A_7, inout tri PMOD_1A_8, inout tri PMOD_1A_9, inout tri PMOD_1A_10,
	input logic BTN_N, inout tri RX, inout tri TX);
	wire [7:0] gpio0pins, gpio1pins, gpio2pins;
	wire clk;
	wire trap;
	wire power_on_reset;

	// Reset button
	assign power_on_reset = !BTN_N;

	// PMOD2
	assign PMOD_2_7 = gpio0pins[0];
	assign gpio0pins[0] = PMOD_2_7;
	assign PMOD_2_1 = gpio0pins[1];
	assign gpio0pins[1] = PMOD_2_1;
	assign PMOD_2_3 = gpio0pins[2];
	assign gpio0pins[2] = PMOD_2_3;
	assign PMOD_2_2 = gpio0pins[3];
	assign gpio0pins[3] = PMOD_2_2;
	assign PMOD_2_8 = gpio0pins[4];
	assign gpio0pins[4] = PMOD_2_8;
	assign PMOD_2_9 = gpio0pins[5];
	assign gpio0pins[5] = PMOD_2_9;
	assign PMOD_2_4 = gpio0pins[6];
	assign gpio0pins[6] = PMOD_2_4;
	assign PMOD_2_10 = gpio0pins[7];
	assign gpio0pins[7] = PMOD_2_10;

	// PMOD1A
	assign PMOD_1A_7 = gpio1pins[0];
	assign gpio1pins[0] = PMOD_1A_7;
	assign PMOD_1A_1 = gpio1pins[1];
	assign gpio1pins[1] = PMOD_1A_1;
	assign PMOD_1A_3 = gpio1pins[2];
	assign gpio1pins[2] = PMOD_1A_3;
	assign PMOD_1A_2 = gpio1pins[3];
	assign gpio1pins[3] = PMOD_1A_2;
	assign PMOD_1A_8 = gpio1pins[4];
	assign gpio1pins[4] = PMOD_1A_8;
	assign PMOD_1A_9 = gpio1pins[5];
	assign gpio1pins[5] = PMOD_1A_9;
	assign PMOD_1A_4 = gpio1pins[6];
	assign gpio1pins[6] = PMOD_1A_4;
	assign PMOD_1A_10 = gpio1pins[7];
	assign gpio1pins[7] = PMOD_1A_10;

	// Serial
	assign TX = gpio2pins[0];
	assign gpio2pins[0] = TX;
	assign RX = gpio2pins[1];
	assign gpio2pins[1] = RX;

	clkdiv #(.CLKDIV(CLKDIV)) clkdiv(.*);
	system #(.ROMSIZE(ROMSIZE), .RAMSIZE(RAMSIZE)) system(.*);
endmodule

`end_keywords

