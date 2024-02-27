`begin_keywords "1800-2009"

// GPIO. 8 or 16 bits (only 8 bits implemented so far).
// odr: output data, idr: input data, ddr: data direction, pr: pullup.
module gpio #(parameter WIDTH = 8) (
	inout tri logic [WIDTH-1:0] pins,
	output logic [WIDTH-1:0] odr_out, ddr_out, idr_out, pr_out,
	input logic [WIDTH-1:0] odr_in, ddr_in, pr_in,
	input logic odr_write, ddr_write, pr_write,
	input logic clk, input logic reset);

	logic [WIDTH-1:0] odr, ddr, pr;

	always @(posedge clk)
	begin
		if(reset)
			ddr = 0;
		else if (ddr_write)
			ddr = ddr_in;

		if(odr_write)
			odr = odr_in;
		if(pr_write)
			pr = pr_in;
	end
	always_comb
	begin
		ddr_out = ddr;
		odr_out = odr;
		pr_out = pr;
		idr_out = pins;
	end

	// Todo: parametrize this. Implement pull-up via pr. Implement interrupts.
	assign pins[0] = ddr[0] ? odr[0] : 1'bZ;
	assign pins[1] = ddr[1] ? odr[1] : 1'bZ;
	assign pins[2] = ddr[2] ? odr[2] : 1'bZ;
	assign pins[3] = ddr[3] ? odr[3] : 1'bZ;
	assign pins[4] = ddr[4] ? odr[4] : 1'bZ;
	assign pins[5] = ddr[5] ? odr[5] : 1'bZ;
	assign pins[6] = ddr[6] ? odr[6] : 1'bZ;
	assign pins[7] = ddr[7] ? odr[7] : 1'bZ;

endmodule

`end_keywords

