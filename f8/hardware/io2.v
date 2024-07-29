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
	localparam WATCHDOG_CONFIG_ADDR = WATCHDOGADDRBASE + 0;
	localparam WATCHDOG_COUNTER_ADDR = WATCHDOGADDRBASE + 2;
	logic [1:0] watchdog_counter_read;
	logic [1:0] watchdog_counter_write;
	logic watchdog_config_read;
	logic watchdog_config_write;
	logic watchdog_zero_write;
	logic [7:0] watchdog_config_dread;
	logic [15:0] watchdog_counter_dread;
	logic [7:0] watchdog_config_dwrite;
	logic [15:0] watchdog_counter_dwrite;
	always_comb
	begin
		watchdog_config_write = write_en_even && (write_addr_even == WATCHDOG_CONFIG_ADDR / 2);
		watchdog_counter_write = {write_en_even && (write_addr_even == WATCHDOG_COUNTER_ADDR / 2), write_en_odd && (write_addr_odd == (WATCHDOG_COUNTER_ADDR + 1) / 2)};
		watchdog_zero_write = write_en_even && (write_addr_even == 0);
	end
	watchdog watchdog(.counter_out(watchdog_counter_dread), .config_out(watchdog_config_dread),
		.counter_in({write_data_odd, write_data_even}), .config_in(write_data_even),
		.counter_write(watchdog_counter_write), .config_write(watchdog_config_write), .zero_write(watchdog_zero_write), .*);

	// Interrupt controller
	localparam IRQCTRL_ENABLE_ADDR = IRQCTRLADDRBASE + 0;
	localparam IRQCTRL_ACTIVE_ADDR = IRQCTRLADDRBASE + 2;
	wire irqctrl_enable_write, irqctrl_active_write;
	wire timer_counter_write[1:0], timer_reload_write[1:0], timer_config_write;
	logic [1:0] irqctrl_enable_dread, irqctrl_active_dread;
	logic [1:0] irqctrl_enable_dwrite, irqctrl_active_dwrite;
	logic [1:0] interrupts;
	assign irqctrl_enable_write = write_en_even && (write_addr_even == IRQCTRL_ENABLE_ADDR / 2);
	assign irqctrl_active_write = write_en_even && (write_addr_even == IRQCTRL_ACTIVE_ADDR / 2);
	interruptcontroller #(.NUM_INPUTS(NUM_IRQ)) irqctrl(.int_out(interrupt), .enable_out(irqctrl_enable_dread), .active_out(irqctrl_active_dread),
		.enable_in(write_data_even[1:0]), .active_in(write_data_even[1:0]), .enable_in_write(irqctrl_enable_write), .active_in_write(irqctrl_active_write),
		.in(interrupts), .*);

	// Timer 0
	localparam TIMER0_CONFIG_ADDR = TIMER0ADDRBASE + 0;
	localparam TIMER0_COUNTER_ADDR = TIMER0ADDRBASE + 2;
	localparam TIMER0_RELOAD_ADDR = TIMER0ADDRBASE + 4;
	localparam TIMER0_COMPARE_ADDR = TIMER0ADDRBASE + 5;
	wire timer0ovirq, timer0cmpirq;
	logic [0:0] timer0in;
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
		timer0_config_write = write_en_even && (write_addr_even == TIMER0_CONFIG_ADDR / 2);
		timer0_counter_write = {write_en_even && (write_addr_even == TIMER0_COUNTER_ADDR / 2), write_en_odd && (write_addr_odd == (TIMER0_COUNTER_ADDR + 1) / 2)};
		timer0_reload_write = {write_en_even && (write_addr_even == TIMER0_RELOAD_ADDR / 2), write_en_odd && (write_addr_odd == (TIMER0_RELOAD_ADDR + 1) / 2)};
		timer0_compare_write = {write_en_even && (write_addr_even == TIMER0_COMPARE_ADDR / 2), write_en_odd && (write_addr_odd == (TIMER0_COMPARE_ADDR + 1) / 2)};
	end
	timer timer0(.counter_out(timer0_counter_dread), .reload_out(timer0_reload_dread), .compare_out(timer0_compare_dread), .config_out(timer0_config_dread),
		.overflow_int(timer0ovirq), .compare_int(timer0cmpirq), .counter_in({write_data_odd, write_data_even}), .reload_in({write_data_odd, write_data_even}), .compare_in({write_data_odd, write_data_even}), .config_in(write_data_even),
		.counter_write(timer0_counter_write), .reload_write(timer0_reload_write), .compare_write(timer0_compare_write), .config_write(timer0_config_write), .in(timer0in), .*);

	// GPIO0
	localparam GPIO0_DDR_ADDR = GPIO0ADDDRBASE + 0;
	localparam GPIO0_ODR_ADDR = GPIO0ADDDRBASE + 2;
	localparam GPIO0_IDR_ADDR = GPIO0ADDDRBASE + 4;
	localparam GPIO0_PR_ADDR = GPIO0ADDDRBASE + 6;
	wire gpio0_ddr_write, gpio0_odr_write, gpio0_pr_write;
	logic [7:0] gpio0_ddr_dread, gpio0_odr_dread, gpio0_idr_dread, gpio0_pr_dread;
	assign gpio0_ddr_write = write_en_even && (write_addr_even == GPIO0_DDR_ADDR / 2);
	assign gpio0_odr_write = write_en_even && (write_addr_even == GPIO0_ODR_ADDR / 2);
	assign gpio0_pr_write = write_en_even && (write_addr_even == GPIO0_PR_ADDR / 2);
	gpio gpio0(.pins(gpio0pins), .ddr_out(gpio0_ddr_dread), .odr_out(gpio0_odr_dread), .idr_out(gpio0_idr_dread), .pr_out(gpio0_pr_dread),
		.ddr_in(write_data_even), .odr_in(write_data_even), .pr_in(write_data_even),
		.ddr_write(gpio0_ddr_write), .odr_write(gpio0_odr_write), .pr_write(gpio0_pr_write),
		.*);

	// GPIO1
	localparam GPIO1_DDR_ADDR = GPIO1ADDDRBASE + 0;
	localparam GPIO1_ODR_ADDR = GPIO1ADDDRBASE + 2;
	localparam GPIO1_IDR_ADDR = GPIO1ADDDRBASE + 4;
	localparam GPIO1_PR_ADDR = GPIO1ADDDRBASE + 6;
	/*wire gpio1_ddr_write, gpio1_odr_write, gpio1_pr_write;
	logic [7:0] gpio1_ddr_dread, gpio1_odr_dread, gpio1_idr_dread, gpio1_pr_dread;
	assign gpio1_ddr_write = write_en_even && (write_addr_even == GPIO1_DDR_ADDR / 2);
	assign gpio1_odr_write = write_en_even && (write_addr_even == GPIO1_ODR_ADDR / 2);
	assign gpio1_pr_write = write_en_even && (write_addr_even == GPIO1_PR_ADDR / 2);
	gpio gpio1(.pins(gpio1pins), .ddr_out(gpio1_ddr_dread), .odr_out(gpio1_odr_dread), .idr_out(gpio1_idr_dread), .pr_out(gpio1_pr_dread),
		.ddr_in(write_data_even), .odr_in(write_data_even), .pr_in(write_data_even),
		.ddr_write(gpio1_ddr_write), .odr_write(gpio1_odr_write), .pr_write(gpio1_pr_write),
		.*);*/

	// GPIO2
	localparam GPIO2_DDR_ADDR = GPIO2ADDDRBASE + 0;
	localparam GPIO2_ODR_ADDR = GPIO2ADDDRBASE + 2;
	localparam GPIO2_IDR_ADDR = GPIO2ADDDRBASE + 4;
	localparam GPIO2_PR_ADDR = GPIO2ADDDRBASE + 6;
	wire gpio2_ddr_write, gpio2_odr_write, gpio2_pr_write;
	logic [7:0] gpio2_ddr_dread, gpio2_odr_dread, gpio2_idr_dread, gpio2_pr_dread;
	assign gpio2_ddr_write = write_en_even && (write_addr_even == GPIO2_DDR_ADDR / 2);
	assign gpio2_odr_write = write_en_even && (write_addr_even == GPIO2_ODR_ADDR / 2);
	assign gpio2_pr_write = write_en_even && (write_addr_even == GPIO2_PR_ADDR / 2);
	gpio gpio2(.pins(gpio2pins), .ddr_out(gpio2_ddr_dread), .odr_out(gpio2_odr_dread), .idr_out(gpio2_idr_dread), .pr_out(gpio2_pr_dread),
		.ddr_in(write_data_even), .odr_in(write_data_even), .pr_in(write_data_even),
		.ddr_write(gpio2_ddr_write), .odr_write(gpio2_odr_write), .pr_write(gpio2_pr_write),
		.*);

	always @(posedge clk)
	begin
		if(read_addr_even == write_addr_even && write_en_even)
			read_data_even = write_data_even;
		else case(read_addr_even)
		WATCHDOG_CONFIG_ADDR / 2:
			read_data_even = watchdog_config_dread;
		WATCHDOG_COUNTER_ADDR / 2:
			read_data_even = watchdog_counter_dread[7:0];
		IRQCTRL_ENABLE_ADDR / 2:
			read_data_even = {'x, irqctrl_enable_dread[NUM_IRQ-1:0]};
		IRQCTRL_ACTIVE_ADDR / 2:
			read_data_even = {'x, irqctrl_active_dread[NUM_IRQ-1:0]};
		TIMER0_CONFIG_ADDR / 2:
			read_data_even = timer0_config_dread;
		TIMER0_COUNTER_ADDR / 2:
			read_data_even = timer0_counter_dread[7:0];
		TIMER0_RELOAD_ADDR / 2:
			read_data_even = timer0_reload_dread[7:0];
		TIMER0_COMPARE_ADDR / 2:
			read_data_even = timer0_compare_dread[7:0];
		GPIO0_DDR_ADDR / 2:
			read_data_even = gpio0_ddr_dread[7:0];
		GPIO0_ODR_ADDR / 2:
			read_data_even = gpio0_odr_dread[7:0];
		GPIO0_IDR_ADDR / 2:
			read_data_even = gpio0_idr_dread[7:0];
		GPIO0_PR_ADDR / 2:
			read_data_even = gpio0_pr_dread[7:0];
		GPIO2_DDR_ADDR / 2:
			read_data_even = gpio2_ddr_dread[7:0];
		GPIO2_ODR_ADDR / 2:
			read_data_even = gpio2_odr_dread[7:0];
		GPIO2_IDR_ADDR / 2:
			read_data_even = gpio2_idr_dread[7:0];
		GPIO2_PR_ADDR / 2:
			read_data_even = gpio2_pr_dread[7:0];
		default:
			read_data_even = 'x;
		endcase

		if(read_addr_odd == write_addr_odd && write_en_odd)
			read_data_odd = write_data_odd;
		else case(read_addr_odd)
		(WATCHDOG_COUNTER_ADDR + 1) / 2:
			read_data_odd = watchdog_counter_dread[15:8];
		(TIMER0_COUNTER_ADDR + 1) / 2:
			read_data_odd = timer0_counter_dread[15:8];
		(TIMER0_RELOAD_ADDR + 1) / 2:
			read_data_odd = timer0_reload_dread[15:8];
		(TIMER0_COMPARE_ADDR + 1) / 2:
			read_data_odd = timer0_compare_dread[15:8];
		default:
			read_data_odd = 'x;
		endcase
	end
	
endmodule

`end_keywords

