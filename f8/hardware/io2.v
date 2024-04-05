`begin_keywords "1800-2009"

`include "interruptcontroller.v"
`include "timer.v"
`include "gpio.v"
`include "watchdog2.v"

module iosystem
	#(parameter
	NUM_IRQ = 2,
	IRQCTRLADDRBASE = 16'h0010,
	TIMER0ADDRBASE = 16'h0018,
	WATCHDOGADDRBASE = 16'h0020,
	GPIO0ADDDRBASE = 16'h0028,
	GPIO1ADDDRBASE = 16'h0030,
	GPIO2ADDDRBASE = 16'h0038)
	
	(input logic [14:0] read_addr_even, output logic [7:0] read_data_even, input logic [14:0] write_addr_even, input logic [7:0] write_data_even, input logic write_en_even,
	input logic [14:0] read_addr_odd, output logic [7:0] read_data_odd, input logic [14:0] write_addr_odd, input logic [7:0] write_data_odd, input logic write_en_odd,
	output logic interrupt, input logic clk, output logic reset, input logic trap, input logic power_on_reset,
	inout tri logic [7:0] gpio0pins, inout tri logic [7:0] gpio1pins, inout tri logic [7:0] gpio2pins);

	// Watchdog
	logic [1:0] watchdog_counter_read, watchdog_reload_read;
	logic [1:0] watchdog_counter_write, watchdog_reload_write;
	logic watchdog_config_read;
	logic watchdog_config_write;
	logic watchdog_zero_write;
	logic [7:0] watchdog_config_dread;
	logic [15:0] watchdog_counter_dread, watchdog_reload_dread;
	logic [7:0] watchdog_config_dwrite;
	logic [15:0] watchdog_counter_dwrite, watchdog_reload_dwrite;
	always_comb
	begin
		watchdog_config_read = (read_addr_even == WATCHDOGADDRBASE / 2);
		watchdog_config_write = write_en_even && (write_addr_even == WATCHDOGADDRBASE / 2);
		watchdog_counter_read = {read_addr_even == (WATCHDOGADDRBASE + 2) / 2, read_addr_odd == (WATCHDOGADDRBASE + 3) / 2};
		watchdog_counter_write = {write_en_even && (write_addr_even == (WATCHDOGADDRBASE + 2) / 2), write_en_odd && (write_addr_odd == (WATCHDOGADDRBASE + 3) / 2)};
		watchdog_reload_read = {read_addr_even == (WATCHDOGADDRBASE + 4) / 2, read_addr_odd == (WATCHDOGADDRBASE + 5) / 2};
		watchdog_reload_write = {write_en_even && (write_addr_even == (WATCHDOGADDRBASE + 4) / 2), write_en_odd && (write_addr_odd == (WATCHDOGADDRBASE + 5) / 2)};
		watchdog_zero_write = write_en_even && (write_addr_even == 0);
	end
	watchdog watchdog(.counter_out(watchdog_counter_dread), .reload_out(watchdog_reload_dread), .config_out(watchdog_config_dread),
		.counter_in({write_data_odd, write_data_even}), .reload_in({write_data_odd, write_data_even}), .config_in(write_data_even),
		.counter_write(watchdog_counter_write), .reload_write(watchdog_reload_write), .config_write(watchdog_config_write), .zero_write(watchdog_zero_write), .*);

	// Interrupt controller
	wire irqctrl_enable_read, irqctrl_active_read;
	wire irqctrl_enable_write, irqctrl_active_write;
	wire timer_counter_write[1:0], timer_reload_write[1:0], timer_config_write;
	logic [1:0] irqctrl_enable_dread, irqctrl_active_dread;
	logic [1:0] irqctrl_enable_dwrite, irqctrl_active_dwrite;
	logic [1:0] interrupts;
	assign irqctrl_enable_read = (read_addr_even == IRQCTRLADDRBASE / 2);
	assign irqctrl_active_read = (read_addr_even == (IRQCTRLADDRBASE + 2) / 2);
	assign irqctrl_enable_write = write_en_even && (write_addr_even == IRQCTRLADDRBASE / 2);
	assign irqctrl_active_write = write_en_even && (write_addr_even == (IRQCTRLADDRBASE + 2) / 2);
	interruptcontroller #(.NUM_INPUTS(NUM_IRQ))
		irqctrl(.int_out(interrupt), .enable_out(irqctrl_enable_dread), .active_out(irqctrl_active_dread), .enable_in(write_data_even[1:0]), .active_in(write_data_even[1:0]), .enable_in_write(irqctrl_enable_write), .active_in_write(irqctrl_active_write),
		.in(interrupts), .*);

	// Timer 0
	wire timer0ovirq, timer0cmpirq;
	logic [0:0] timer0in;
	logic timer0_config_read;
	logic [1:0] timer0_counter_read, timer0_reload_read, timer0_compare_read;
	logic [1:0] timer0_counter_write, timer0_reload_write, timer0_compare_write;
	logic timer0_config_write;
	logic [7:0] timer0_config_dread;
	logic [15:0] timer0_counter_dread, timer0_reload_dread, timer0_compare_dread;
	logic [7:0] timer0_config_dwrite;
	logic [15:0] timer0_counter_dwrite, timer0_reload_dwrite, timer0_compare_dwrite;
	assign interrupts[0] = timer0ovirq;
	assign interrupts[1] = timer0cmpirq;
	always_comb
	begin
		timer0_config_read = (read_addr_even == TIMER0ADDRBASE / 2);
		timer0_config_write = write_en_even && (write_addr_even == TIMER0ADDRBASE / 2);
		timer0_counter_read = {read_addr_even == (TIMER0ADDRBASE + 2) / 2, read_addr_odd == (TIMER0ADDRBASE + 3) / 2};
		timer0_counter_write = {write_en_even && (write_addr_even == (TIMER0ADDRBASE + 2) / 2), write_en_odd && (write_addr_odd == (TIMER0ADDRBASE + 3) / 2)};
		timer0_reload_read = {read_addr_even == (TIMER0ADDRBASE + 4), read_addr_odd == (TIMER0ADDRBASE + 5) / 2};
		timer0_reload_write = {write_en_even && (write_addr_even == (TIMER0ADDRBASE + 4) / 2), write_en_odd && (write_addr_odd == (TIMER0ADDRBASE + 5) / 2)};
		timer0_compare_read = {read_addr_even == (TIMER0ADDRBASE + 6) / 2,read_addr_odd == (TIMER0ADDRBASE + 7) / 2};
		timer0_compare_write = {write_en_even && (write_addr_even == (TIMER0ADDRBASE + 6) / 2), write_en_odd && (write_addr_odd == (TIMER0ADDRBASE + 7) / 2)};
	end
	timer timer0(.counter_out(timer0_counter_dread), .reload_out(timer0_reload_dread), .compare_out(timer0_compare_dread), .config_out(timer0_config_dread),
		.overflow_int(timer0ovirq), .compare_int(timer0cmpirq), .counter_in({write_data_odd, write_data_even}), .reload_in({write_data_odd, write_data_even}), .compare_in({write_data_odd, write_data_even}), .config_in(write_data_even),
		.counter_write(timer0_counter_write), .reload_write(timer0_reload_write), .compare_write(timer0_compare_write), .config_write(timer0_config_write), .in(timer0in), .*);

	// GPIO0
	wire gpio0_ddr_read, gpio0_odr_read, gpio0_idr_read, gpio0_pr_read;
	wire gpio0_ddr_write, gpio0_odr_write, gpio0_pr_write;
	logic [7:0] gpio0_ddr_dread, gpio0_odr_dread, gpio0_idr_dread, gpio0_pr_dread;
	assign gpio0_ddr_read = (read_addr_even == GPIO0ADDDRBASE / 2);
	assign gpio0_odr_read = (read_addr_even == (GPIO0ADDDRBASE + 2) / 2);
	assign gpio0_idr_read = (read_addr_even == (GPIO0ADDDRBASE + 4) / 2);
	assign gpio0_pr_read = (read_addr_even == (GPIO0ADDDRBASE + 6) / 2);
	assign gpio0_ddr_write = write_en_even && (write_addr_even == GPIO0ADDDRBASE / 2);
	assign gpio0_odr_write = write_en_even && (write_addr_even == (GPIO0ADDDRBASE + 2) / 2);
	assign gpio0_pr_write = write_en_even && (write_addr_even == (GPIO0ADDDRBASE + 6) / 2);
	gpio gpio0(.pins(gpio0pins), .ddr_out(gpio0_ddr_dread), .odr_out(gpio0_odr_dread), .idr_out(gpio0_idr_dread), .pr_out(gpio0_pr_dread),
		.ddr_in(write_data_even), .odr_in(write_data_even), .pr_in(write_data_even),
		.ddr_write(gpio0_ddr_write), .odr_write(gpio0_odr_write), .pr_write(gpio0_pr_write),
		.*);

	// GPIO1
	wire gpio1_ddr_read, gpio1_odr_read, gpio1_idr_read, gpio1_pr_read;
	wire gpio1_ddr_write, gpio1_odr_write, gpio1_pr_write;
	logic [7:0] gpio1_ddr_dread, gpio1_odr_dread, gpio1_idr_dread, gpio1_pr_dread;
	assign gpio1_ddr_read = (read_addr_even == GPIO1ADDDRBASE / 2);
	assign gpio1_odr_read = (read_addr_even == (GPIO1ADDDRBASE + 2) / 2);
	assign gpio1_idr_read = (read_addr_even == (GPIO1ADDDRBASE + 4) / 2);
	assign gpio1_pr_read = (read_addr_even == (GPIO1ADDDRBASE + 6) / 2);
	assign gpio1_ddr_write = write_en_even && (write_addr_even == (GPIO1ADDDRBASE) / 2);
	assign gpio1_odr_write = write_en_even && (write_addr_even == (GPIO1ADDDRBASE + 2) / 2);
	assign gpio1_pr_write = write_en_even && (write_addr_even == (GPIO1ADDDRBASE + 6) / 2);
	gpio gpio1(.pins(gpio1pins), .ddr_out(gpio1_ddr_dread), .odr_out(gpio1_odr_dread), .idr_out(gpio1_idr_dread), .pr_out(gpio1_pr_dread),
		.ddr_in(write_data_even), .odr_in(write_data_even), .pr_in(write_data_even),
		.ddr_write(gpio1_ddr_write), .odr_write(gpio1_odr_write), .pr_write(gpio1_pr_write),
		.*);

	// GPIO2
	wire gpio2_ddr_read, gpio2_odr_read, gpio2_idr_read, gpio2_pr_read;
	wire gpio2_ddr_write, gpio2_odr_write, gpio2_pr_write;
	logic [7:0] gpio2_ddr_dread, gpio2_odr_dread, gpio2_idr_dread, gpio2_pr_dread;
	assign gpio2_ddr_read = (read_addr_even == GPIO2ADDDRBASE / 2);
	assign gpio2_odr_read = (read_addr_even == (GPIO2ADDDRBASE + 2) / 2);
	assign gpio2_idr_read = (read_addr_even == (GPIO2ADDDRBASE + 4) / 2);
	assign gpio2_pr_read = (read_addr_even == (GPIO2ADDDRBASE + 6) / 2);
	assign gpio2_ddr_write = write_en_even && (write_addr_even == GPIO2ADDDRBASE / 2);
	assign gpio2_odr_write = write_en_even && (write_addr_even == (GPIO2ADDDRBASE + 2) / 2);
	assign gpio2_pr_write = write_en_even && (write_addr_even == (GPIO2ADDDRBASE + 6) / 2);
	gpio gpio2(.pins(gpio2pins), .ddr_out(gpio2_ddr_dread), .odr_out(gpio2_odr_dread), .idr_out(gpio2_idr_dread), .pr_out(gpio2_pr_dread),
		.ddr_in(write_data_even), .odr_in(write_data_even), .pr_in(write_data_even),
		.ddr_write(gpio2_ddr_write), .odr_write(gpio2_odr_write), .pr_write(gpio2_pr_write),
		.*);

	always @(posedge clk)
	begin
		if(read_addr_even == write_addr_even)
			read_data_even = write_data_even;
		else if(watchdog_config_read)
			read_data_even = watchdog_config_dread;
		else if(watchdog_counter_read[0])
			read_data_even = watchdog_counter_dread[7:0];
		else if(watchdog_reload_read[0])
			read_data_even = watchdog_reload_dread[7:0];
		else if(timer0_config_read)
			read_data_even = timer0_config_dread;
		else if(timer0_counter_read[0])
			read_data_even = timer0_counter_dread[7:0];
		else if(timer0_reload_read[0])
			read_data_even = timer0_reload_dread[7:0];
		else if(timer0_compare_read[0])
			read_data_even = timer0_compare_dread[7:0];
		else if(irqctrl_enable_read)
			read_data_even = {'x, irqctrl_enable_dread[NUM_IRQ-1:0]};
		else if(irqctrl_active_read)
			read_data_even = {'x, irqctrl_active_dread[NUM_IRQ-1:0]};
		else if(gpio0_ddr_read)
			read_data_even = gpio0_ddr_dread[7:0];
		else if(gpio0_odr_read)
			read_data_even = gpio0_odr_dread[7:0];
		else if(gpio0_idr_read)
			read_data_even = gpio0_idr_dread[7:0];
		else if(gpio0_pr_read)
			read_data_even = gpio0_pr_dread[7:0];
		else if(gpio1_ddr_read)
			read_data_even = gpio1_ddr_dread[7:0];
		else if(gpio1_odr_read)
			read_data_even = gpio1_odr_dread[7:0];
		else if(gpio1_idr_read)
			read_data_even = gpio1_idr_dread[7:0];
		else if(gpio1_pr_read)
			read_data_even = gpio1_pr_dread[7:0];
		else if(gpio2_ddr_read)
			read_data_even = gpio2_ddr_dread[7:0];
		else if(gpio2_odr_read)
			read_data_even = gpio2_odr_dread[7:0];
		else if(gpio2_idr_read)
			read_data_even = gpio2_idr_dread[7:0];
		else if(gpio2_pr_read)
			read_data_even = gpio2_pr_dread[7:0];
		else
			read_data_even = 'x;
		if (read_addr_odd == write_addr_odd)
			read_data_odd = write_data_odd;
		else if(watchdog_counter_read[1])
			read_data_even = watchdog_counter_dread[15:8];
		else if(watchdog_reload_read[1])
			read_data_even = watchdog_reload_dread[15:8];
		else if(timer0_counter_read[1])
			read_data_even = timer0_counter_dread[15:8];
		else if(timer0_reload_read[1])
			read_data_even = timer0_reload_dread[15:8];
		else if(timer0_compare_read[1])
			read_data_even = timer0_compare_dread[15:8];
		else
			read_data_odd = 'x;
	end
	
endmodule

`end_keywords

