`begin_keywords "1800-2009"

// Watchdog timer.
// Input: system clock.
// Prescaler: /16.
// On overflow: watchdog reset. On trap: trap reset.
// I/O registers: counter (16 bit), reload (16 bit), config (8 bit - lowest bit enables watchdog, next three bits indicate type of most recent reset)
module watchdog (output logic reset, output logic [15:0] counter_out, reload_out, output logic [7:0] config_out, input logic [15:0] counter_in, reload_in,  input logic [7:0] config_in, input logic [1:0] counter_write, reload_write, input config_write, zero_write, input logic clk, power_on_reset, trap);
	logic [7:0] configreg;
	logic [15:0] countreg, reloadreg;
	logic internal_reset;
	logic count_now, overflow_int, prescaled_count_now;

	assign count_now = configreg[0];

	// Prescaler
	logic[3:0] presccounter;
	always @(posedge clk)
	begin
		if(reset)
			presccounter = 0;
		else if(count_now)
			presccounter++;
	end
	assign prescaled_count_now = (presccounter == 4'hf);

	// Watchdog counter
	always @(posedge clk)
	begin
		if(!prescaled_count_now)
		begin
			overflow_int = 0;
		end
		else
		begin
			if (countreg == 16'hffff)
			begin
				overflow_int = 1;
				countreg = reloadreg;
			end
			else
			begin
				overflow_int = 0;
				countreg++;
			end
		end
		if (counter_write[0])
			countreg[7:0] = counter_in[7:0];
		if (counter_write[1])
			countreg[15:8] = counter_in[15:8];
	end
	assign counter_out = countreg;
	assign reload_out = reloadreg;

	// Config
	always @(posedge clk)
	begin
		if(power_on_reset)
			configreg[3:0] = 0;
		else if(reset)
			configreg[0] = 0;
		else if(config_write)
			configreg[7:0] = config_in[7:0];
		else
		begin
			if(overflow_int)
				configreg[1] = 1;
			if(trap)
				configreg[2] = 1;
			if(zero_write)
				configreg[3] = 1;
		end
		if(reload_write)
		begin
			if(reload_write[0])
				reloadreg[7:0] = reload_in[7:0];
			if(reload_write[1])
				reloadreg[15:8] = reload_in[15:8];
		end
		internal_reset = overflow_int || trap || zero_write;
	end
	assign config_out = configreg;
	assign reset = power_on_reset || internal_reset;
endmodule

`end_keywords
