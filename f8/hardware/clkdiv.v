`begin_keywords "1800-2009"

// Clock divider that divides the input clock CLK by CLKDIV (1 to 8).
module clkdiv #(parameter CLKDIV = 4) (output logic clk, input logic CLK);
	logic[3:0] counter = 4'h0;

	always_ff @(posedge CLK)
	begin
		if(counter == CLKDIV - 1)
			counter <= 4'h0;
		else
			counter <= counter + 1;
	end
	assign clk = (counter < CLKDIV / 2);
endmodule

`end_keywords

