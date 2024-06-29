`begin_keywords "1800-2009"

// 16 bit up-counting timer. All inputs are assumed to be synchronized.
// Input: system clock or other (e.g. slower clock, i/o).
// Prescaler: /1, /4, /16, /64.
// Interrupt on overflow and compare.
// On overflow: reset counter to reload value.
// I/O registers: counter (16 bit), reload (16 bit), config (8 bit - lower 4 bits select input, next two bits select prescaler, upper 2 bits unused and read as 0)
module timer #(parameter NUM_INPUTS = 1) (output logic [15:0] counter_out, reload_out, compare_out, output logic [7:0] config_out, output logic overflow_int, compare_int, input logic [15:0] counter_in, reload_in, compare_in, input logic [7:0] config_in, input logic [1:0] counter_write, reload_write, compare_write, input config_write, input logic [NUM_INPUTS-1:0] in, input logic clk, input logic reset);
	wire [3:0] clksel;
	wire [1:0] prescsel;

	logic [5:0] configreg;
	logic [15:0] countreg, reloadreg, comparereg;

	logic count_now, prescaled_count_now;
	logic [NUM_INPUTS-1:0] in_edge, old_in;

	// Detect edges on (synchronized) input signals
	always @(posedge clk)
	begin
		old_in <= in;
	end
	assign in_edge = in & ~old_in;

	// Clock select
	always_comb
	begin
		if (clksel == 0)
			count_now = 0;
		else if (clksel == 1)
			count_now = 1;
		else
			count_now = in_edge[clksel - 2];
	end

	// Prescaler
	logic[5:0] presccounter;
	always @(posedge clk)
	begin
		if(!count_now)
			prescaled_count_now = 0;
		else
		begin
			prescaled_count_now = (prescsel == 0 ||
				prescsel == 1 && !presccounter[1:0] ||
				prescsel == 2 && !presccounter[3:0] ||
				prescsel == 3 && !presccounter[5:0]);
			presccounter++;
		end
	end

	// Counter
	always @(posedge clk)
	begin
		if(!prescaled_count_now)
		begin
			overflow_int = 0;
			compare_int = 0;
		end
		else
		begin
			if(countreg == 16'hffff)
			begin
				overflow_int = 1;
				countreg = reloadreg;
			end
			else
			begin
				overflow_int = 0;
				countreg++;
			end
			compare_int = (countreg == comparereg);
		end
		if(counter_write[0])
			countreg[7:0] = counter_in[7:0];
		if(counter_write[1])
			countreg[15:8] = counter_in[15:8];
	end
	assign counter_out = countreg;
	assign reload_out = reloadreg;
	assign compare_out = comparereg;

	// Config
	always @(posedge clk)
	begin
		if(reset)
			configreg[5:0] = 6'b000000;
		else if(config_write)
			configreg[5:0] = config_in[5:0];

		if(reload_write[0])
			reloadreg[7:0] = reload_in[7:0];
		if(reload_write[1])
			reloadreg[15:8] = reload_in[15:8];

		if(compare_write[0])
			comparereg[7:0] = compare_in[7:0];
		if(compare_write[1])
			comparereg[15:8] = compare_in[15:8];
	end
	assign config_out = {2'b00, configreg};
	assign prescsel = configreg[5:4];
	assign clksel = configreg[3:0];
endmodule

`end_keywords

