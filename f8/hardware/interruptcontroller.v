`begin_keywords "1800-2009"

// Interrupt controller for up to 16 interrupt input lines and one interrupt output line
// I/O registers: enable (up to 16 bit), active (up to 16 bit)
module interruptcontroller #(parameter NUM_INPUTS = 1) (output logic int_out, output logic [NUM_INPUTS-1:0] enable_out, active_out, input logic [NUM_INPUTS-1:0] enable_in, active_in, input logic enable_in_write, active_in_write, input logic [NUM_INPUTS-1:0] in, input logic clk, input logic reset);
	logic [NUM_INPUTS-1:0] enablereg, activereg;

	always @(posedge clk)
	begin
		if (reset)
		begin
			enablereg = 0;
			activereg = 0;
		end
		else
		begin
			if (enable_in_write)
				enablereg = enable_in;
			if (active_in_write)
				activereg = active_in;
			activereg |= in & enablereg;
		end
	end
	assign int_out = |activereg;
	assign enable_out = enablereg;
	assign active_out = activereg;

endmodule

`end_keywords

