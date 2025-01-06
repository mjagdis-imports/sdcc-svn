`begin_keywords "1800-2009"

`include "../clkdiv.v"
`include "../system_1.v"

// Test module for use on GateMateA1-EVB FPGA board.

// Default: 8KB ROM, 8 KB RAM, 2 MHz system clock.
module gatematea1evb #(parameter ROMSIZE = 8192, RAMSIZE = 8192, CLKDIV = 5) (input logic CLK,
	inout tri PMOD_1, inout tri PMOD_2, inout tri PMOD_3, inout tri PMOD_4, inout tri PMOD_7, inout tri PMOD_8, inout tri PMOD_9, inout tri PMOD_10,
	input logic BTN_N, inout tri RX, inout tri TX);
	wire [7:0] gpio0pins, gpio1pins, gpio2pins;
	wire clk;
	wire trap;
	wire power_on_reset;

	// Reset button
	assign power_on_reset = !BTN_N;

	// PMOD2
	assign PMOD_7 = gpio0pins[0];
	assign gpio0pins[0] = PMOD_7;
	assign PMOD_1 = gpio0pins[1];
	assign gpio0pins[1] = PMOD_1;
	assign PMOD_3 = gpio0pins[2];
	assign gpio0pins[2] = PMOD_3;
	assign PMOD_2 = gpio0pins[3];
	assign gpio0pins[3] = PMOD_2;
	assign PMOD_8 = gpio0pins[4];
	assign gpio0pins[4] = PMOD_8;
	assign PMOD_9 = gpio0pins[5];
	assign gpio0pins[5] = PMOD_9;
	assign PMOD_4 = gpio0pins[6];
	assign gpio0pins[6] = PMOD_4;
	assign PMOD_10 = gpio0pins[7];
	assign gpio0pins[7] = PMOD_10;

	// Serial
	assign TX = gpio2pins[0];
	assign gpio2pins[0] = TX;
	assign RX = gpio2pins[1];
	assign gpio2pins[1] = RX;

	clkdiv #(.CLKDIV(CLKDIV)) clkdiv(.*);
	system #(.ROMSIZE(ROMSIZE), .RAMSIZE(RAMSIZE)) system(.*);
endmodule

`end_keywords

