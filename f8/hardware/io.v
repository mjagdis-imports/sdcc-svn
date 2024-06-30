`include "interruptcontroller.v"
`include "timer.v"
`include "gpio.v"
`include "watchdog.v"

`begin_keywords "1800-2009"

module iosystem
	#(parameter
	NUM_IRQ = 2,
	IRQCTRLADDRBASE = 16'h0010,
	TIMER0ADDRBASE = 16'h0018,
	WATCHDOGADDRBASE = 16'h0020,
	GPIO0ADDDRBASE = 16'h0028,
	GPIO1ADDDRBASE = 16'h0030,
	GPIO2ADDDRBASE = 16'h0038)
	
	(input logic [15:0] dread_addr, output logic [15:0] dread_data,
	input logic [15:0] dwrite_addr, dwrite_data, input logic [1:0] dwrite_en,
	output logic interrupt, input logic clk, output logic reset, input logic trap, input logic power_on_reset,
	inout tri logic [7:0] gpio0pins, inout tri logic [7:0] gpio1pins, inout tri logic [7:0] gpio2pins);

	wire [15:0] dwrite_addr_even, dwrite_addr_odd;
	wire [15:0] dread_addr_even, dread_addr_odd;
	wire dwrite_en_even, dwrite_en_odd;
	assign dread_addr_even = dread_addr[0] ? dread_addr + 1 : dread_addr;
	assign dread_addr_odd = dread_addr[0] ? dread_addr : dread_addr + 1;
	assign dwrite_addr_even = dwrite_addr[0] ? dwrite_addr + 1 : dwrite_addr;
	assign dwrite_addr_odd = dwrite_addr[0] ? dwrite_addr : dwrite_addr + 1;
	assign dwrite_en_even = dwrite_addr[0] ? dwrite_en[1] : dwrite_en[0];
	assign dwrite_en_odd = dwrite_addr[0] ? dwrite_en[0] : dwrite_en[1];

	// Interrupt controller
	wire irqctrl_enable_read, irqctrl_active_read;
	wire irqctrl_enable_write, irqctrl_active_write;
	wire timer_counter_write[1:0], timer_reload_write[1:0], timer_config_write;
	logic [1:0] irqctrl_enable_dread, irqctrl_active_dread;
	logic [1:0] irqctrl_enable_dwrite, irqctrl_active_dwrite;
	logic [1:0] interrupts;
	assign irqctrl_enable_read = (dread_addr_even == IRQCTRLADDRBASE);
	assign irqctrl_active_read = (dread_addr_even == IRQCTRLADDRBASE + 2);
	assign irqctrl_enable_write = dwrite_en_even && (dwrite_addr_even == IRQCTRLADDRBASE);
	assign irqctrl_active_write = dwrite_en_even && (dwrite_addr_even == IRQCTRLADDRBASE + 2);
	interruptcontroller #(.NUM_INPUTS(NUM_IRQ))
		irqctrl(.int_out(interrupt), .enable_out(irqctrl_enable_dread), .active_out(irqctrl_active_dread), .enable_in(dwrite_data[1:0]), .active_in(dwrite_data[1:0]), .enable_in_write(irqctrl_enable_write), .active_in_write(irqctrl_active_write),
		.in(interrupts), .*);

	// Timer 0
	wire timer0ovirq, timer0cmpirq;
	logic [0:0] timer0in;
	logic [1:0] timer0_counter_write, timer0_reload_write, timer0_compare_write;
	logic timer0_config_read;
	logic timer0_config_write;
	logic [7:0] timer0_config_dread;
	logic [15:0] timer0_counter_dread, timer0_reload_dread, timer0_compare_dread;
	logic [7:0] timer0_config_dwrite;
	logic [15:0] timer0_counter_dwrite, timer0_reload_dwrite, timer0_compare_dwrite;
	assign interrupts[0] = timer0ovirq;
	assign interrupts[1] = timer0cmpirq;
	always_comb
	begin
		timer0_config_read = (dread_addr_even == TIMER0ADDRBASE);
		timer0_config_write = dwrite_en_even && (dwrite_addr_even == TIMER0ADDRBASE);
		timer0_counter_write = {dwrite_en_odd && (dwrite_addr_odd == TIMER0ADDRBASE + 3), dwrite_en_even && (dwrite_addr_even == TIMER0ADDRBASE + 2)};
		timer0_reload_write = {dwrite_en_odd && (dwrite_addr_odd == TIMER0ADDRBASE + 5), dwrite_en_even && (dwrite_addr_even == TIMER0ADDRBASE + 4)};
		timer0_compare_write = {dwrite_en_odd && (dwrite_addr_odd == TIMER0ADDRBASE + 7), dwrite_en_even && (dwrite_addr_even == TIMER0ADDRBASE + 6)};
	end
	timer timer0(.counter_out(timer0_counter_dread), .reload_out(timer0_reload_dread), .compare_out(timer0_compare_dread), .config_out(timer0_config_dread),
		.overflow_int(timer0ovirq), .compare_int(timer0cmpirq), .counter_in(dwrite_data), .reload_in(dwrite_data), .compare_in(dwrite_data), .config_in(dwrite_data[7:0]),
		.counter_write(timer0_counter_write), .reload_write(timer0_reload_write), .compare_write(timer0_compare_write), .config_write(timer0_config_write), .in(timer0in), .*);

	// Watchdog
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
		watchdog_config_read = (dread_addr_even == WATCHDOGADDRBASE);
		watchdog_config_write = dwrite_en_even && (dwrite_addr_even == WATCHDOGADDRBASE);
		watchdog_counter_write = {dwrite_en_odd && (dwrite_addr_odd == WATCHDOGADDRBASE + 3), dwrite_en_even && (dwrite_addr_even == WATCHDOGADDRBASE + 2)};
		watchdog_reload_write = {dwrite_en_odd && (dwrite_addr_odd == WATCHDOGADDRBASE + 5), dwrite_en_even && (dwrite_addr_even == WATCHDOGADDRBASE + 4)};
		watchdog_zero_write = dwrite_en_even && (dwrite_addr_even == 0);
	end
	watchdog watchdog(.counter_out(watchdog_counter_dread), .reload_out(watchdog_reload_dread), .config_out(watchdog_config_dread),
		.counter_in(dwrite_data), .reload_in(dwrite_data), .config_in(dwrite_data[7:0]),
		.counter_write(watchdog_counter_write), .reload_write(watchdog_reload_write), .config_write(watchdog_config_write), .zero_write(watchdog_zero_write), .*);

	// GPIO0
	wire gpio0_ddr_read, gpio0_odr_read, gpio0_idr_read, gpio0_pr_read;
	wire gpio0_ddr_write, gpio0_odr_write, gpio0_pr_write;
	logic [7:0] gpio0_ddr_dread, gpio0_odr_dread, gpio0_idr_dread, gpio0_pr_dread;
	assign gpio0_ddr_read = (dread_addr_even == GPIO0ADDDRBASE);
	assign gpio0_odr_read = (dread_addr_even == GPIO0ADDDRBASE + 2);
	assign gpio0_idr_read = (dread_addr_even == GPIO0ADDDRBASE + 4);
	assign gpio0_pr_read = (dread_addr_even == GPIO0ADDDRBASE + 6);
	assign gpio0_ddr_write = dwrite_en_even && (dwrite_addr_even == GPIO0ADDDRBASE);
	assign gpio0_odr_write = dwrite_en_even && (dwrite_addr_even == GPIO0ADDDRBASE + 2);
	assign gpio0_pr_write = dwrite_en_even && (dwrite_addr_even == GPIO0ADDDRBASE + 6);
	gpio gpio0(.pins(gpio0pins), .ddr_out(gpio0_ddr_dread), .odr_out(gpio0_odr_dread), .idr_out(gpio0_idr_dread), .pr_out(gpio0_pr_dread),
		.ddr_in(dwrite_data[7:0]), .odr_in(dwrite_data[7:0]), .pr_in(dwrite_data[7:0]),
		.ddr_write(gpio0_ddr_write), .odr_write(gpio0_odr_write), .pr_write(gpio0_pr_write),
		.*);

	// GPIO1
	wire gpio1_ddr_read, gpio1_odr_read, gpio1_idr_read, gpio1_pr_read;
	wire gpio1_ddr_write, gpio1_odr_write, gpio1_pr_write;
	logic [7:0] gpio1_ddr_dread, gpio1_odr_dread, gpio1_idr_dread, gpio1_pr_dread;
	assign gpio1_ddr_read = (dread_addr_even == GPIO1ADDDRBASE);
	assign gpio1_odr_read = (dread_addr_even == GPIO1ADDDRBASE + 2);
	assign gpio1_idr_read = (dread_addr_even == GPIO1ADDDRBASE + 4);
	assign gpio1_pr_read = (dread_addr_even == GPIO1ADDDRBASE + 6);
	assign gpio1_ddr_write = dwrite_en_even && (dwrite_addr_even == GPIO1ADDDRBASE);
	assign gpio1_odr_write = dwrite_en_even && (dwrite_addr_even == GPIO1ADDDRBASE + 2);
	assign gpio1_pr_write = dwrite_en_even && (dwrite_addr_even == GPIO1ADDDRBASE + 6);
	gpio gpio1(.pins(gpio1pins), .ddr_out(gpio1_ddr_dread), .odr_out(gpio1_odr_dread), .idr_out(gpio1_idr_dread), .pr_out(gpio1_pr_dread),
		.ddr_in(dwrite_data[7:0]), .odr_in(dwrite_data[7:0]), .pr_in(dwrite_data[7:0]),
		.ddr_write(gpio1_ddr_write), .odr_write(gpio1_odr_write), .pr_write(gpio1_pr_write),
		.*);

	// GPIO2
	wire gpio2_ddr_read, gpio2_odr_read, gpio2_idr_read, gpio2_pr_read;
	wire gpio2_ddr_write, gpio2_odr_write, gpio2_pr_write;
	logic [7:0] gpio2_ddr_dread, gpio2_odr_dread, gpio2_idr_dread, gpio2_pr_dread;
	assign gpio2_ddr_read = (dread_addr_even == GPIO2ADDDRBASE);
	assign gpio2_odr_read = (dread_addr_even == GPIO2ADDDRBASE + 2);
	assign gpio2_idr_read = (dread_addr_even == GPIO2ADDDRBASE + 4);
	assign gpio2_pr_read = (dread_addr_even == GPIO2ADDDRBASE + 6);
	assign gpio2_ddr_write = dwrite_en_even && (dwrite_addr_even == GPIO2ADDDRBASE);
	assign gpio2_odr_write = dwrite_en_even && (dwrite_addr_even == GPIO2ADDDRBASE + 2);
	assign gpio2_pr_write = dwrite_en_even && (dwrite_addr_even == GPIO2ADDDRBASE + 6);
	gpio gpio2(.pins(gpio2pins), .ddr_out(gpio2_ddr_dread), .odr_out(gpio2_odr_dread), .idr_out(gpio2_idr_dread), .pr_out(gpio2_pr_dread),
		.ddr_in(dwrite_data[7:0]), .odr_in(dwrite_data[7:0]), .pr_in(dwrite_data[7:0]),
		.ddr_write(gpio2_ddr_write), .odr_write(gpio2_odr_write), .pr_write(gpio2_pr_write),
		.*);

	always @(posedge clk)
	begin
		if(dread_addr_even == dwrite_addr_even && dwrite_en_even)
			dread_data[7:0] = dwrite_data;
		else if(irqctrl_enable_read)
			dread_data[7:0] = {'x, irqctrl_enable_dread[NUM_IRQ-1:0]};
		else if(irqctrl_active_read)
			dread_data[7:0] = {'x, irqctrl_active_dread[NUM_IRQ-1:0]};
		else if(timer0_config_read)
			dread_data[7:0] = {'x, timer0_config_dread[7:0]};
		else if(dread_addr_even == TIMER0ADDRBASE + 2)
			dread_data[7:0] = timer0_counter_dread[7:0];
		else if(dread_addr_even == TIMER0ADDRBASE + 4)
			dread_data[7:0] = timer0_reload_dread[7:0];
		else if(dread_addr_even == TIMER0ADDRBASE + 6)
			dread_data[7:0] = timer0_compare_dread[7:0];
		else if(gpio0_ddr_read)
			dread_data[7:0] = {'x, gpio0_ddr_dread[7:0]};
		else if(gpio0_odr_read)
			dread_data[7:0] = {'x, gpio0_odr_dread[7:0]};
		else if(gpio0_idr_read)
			dread_data[7:0] = {'x, gpio0_idr_dread[7:0]};
		else if(gpio0_pr_read)
			dread_data[7:0] = {'x, gpio0_pr_dread[7:0]};
		else if(gpio1_ddr_read)
			dread_data[7:0] = {'x, gpio1_ddr_dread[7:0]};
		else if(gpio1_odr_read)
			dread_data[7:0] = {'x, gpio1_odr_dread[7:0]};
		else if(gpio1_idr_read)
			dread_data[7:0] = {'x, gpio1_idr_dread[7:0]};
		else if(gpio1_pr_read)
			dread_data[7:0] = {'x, gpio1_pr_dread[7:0]};
		else if(gpio2_ddr_read)
			dread_data[7:0] = {'x, gpio2_ddr_dread[7:0]};
		else if(gpio2_odr_read)
			dread_data[7:0] = {'x, gpio2_odr_dread[7:0]};
		else if(gpio2_idr_read)
			dread_data[7:0] = {'x, gpio2_idr_dread[7:0]};
		else if(gpio2_pr_read)
			dread_data[7:0] = {'x, gpio2_pr_dread[7:0]};
		else
			dread_data[7:0] = 'x;
		if(dread_addr_odd == dwrite_addr_odd && dwrite_en_odd)
			dread_data[15:8] = dwrite_data;
		else if(dread_addr_odd == TIMER0ADDRBASE + 3)
			dread_data[15:8] = timer0_counter_dread[15:8];
		else if(dread_addr_odd == TIMER0ADDRBASE + 5)
			dread_data[15:8] = timer0_reload_dread[15:8];
		else if(dread_addr_odd == TIMER0ADDRBASE + 7)
			dread_data[15:8] = timer0_compare_dread[15:8];
		else
			dread_data[15:8] = 'x;
	end
	
endmodule

`end_keywords

