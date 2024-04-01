`begin_keywords "1800-2009"

// Dual port RAM module suitable for iCE40 4kb embedded block RAM and GateMate block RAM.
module dualportram #(parameter ADDRBITS = 9, DATAWIDTH = 8) (input logic [ADDRBITS-1:0] waddr, input logic [DATAWIDTH-1:0] din, input logic [ADDRBITS-1:0] raddr, output logic [DATAWIDTH-1:0] dout, input logic write_en, input logic clk);
	logic [DATAWIDTH-1:0] mem[(1 << ADDRBITS) - 1 : 0];
  
	always @(posedge clk)
	begin
		if (write_en)
			mem[waddr] <= din;
		if (write_en && waddr == raddr)
			dout <= din;
		else
			dout <= mem[raddr];
	end
endmodule

`end_keywords

