`include "cpu.v"
`include "memory.v"

`begin_keywords "1800-2009"

module clkgen (clk);
	output reg clk;

	initial
		clk = 0;

	always #5 clk = !clk;
endmodule

module testsystem ();
	wire [15:0] iread_addr, dread_addr, dwrite_addr;
	wire [23:0] iread_data;
	wire [15:0] dread_data, dwrite_data;
	wire ivalid;
	wire [1:0] dwrite_en;
	wire clk;
	reg reset;
	wire trap;

	clkgen clkgen(.*);
	memory memory(.*);
	cpu cpu(.*);
	
	initial
	begin
		$dumpfile("test.vcd");
    	$dumpvars(0,testsystem);
    	reset <= 1;
    	#20
    	reset <= 0;
		#780
		$finish;
	end

	always @(posedge trap)
	begin
		$display("ERROR: TRAP");
		#10
		$finish;
	end
endmodule

`end_keywords

