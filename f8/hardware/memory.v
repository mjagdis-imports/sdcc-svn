// Dual port RAM module suitable for iCE40 4kb Embedded Block RAM
module dualportram(din, write_en, waddr, wclk, raddr, rclk, dout);
	parameter addr_width = 9; // 512 x
	parameter data_width = 8; // x 8
	input write_en, wclk, rclk;
	input [addr_width - 1 : 0] waddr, raddr;
	input [data_width - 1 : 0] din;
	output reg [data_width - 1 : 0] dout;

	reg [data_width - 1 : 0] mem[(1 << addr_width) - 1 : 0];
  
	always @(posedge wclk)
	begin
		if (write_en)
			mem[waddr] <= din;
	end

	always @(posedge rclk)
	begin
		dout <= mem[raddr];
	end
endmodule


// Memory subsystem - ideal memory for simulation. TODO: Build an implementation from dualportram above, using ivalid to resolve read conflicts?
// Has three ports (instruction read, data read, data write)
module memory(iread_addr, iread_data, iread_valid, dread_addr, dread_data, dwrite_addr, dwrite_data, dwrite_en, clk);
	parameter ramsize = 32768;
	input [15 : 0] iread_addr;
	output reg [23 : 0] iread_data;
	output reg iread_valid;
	input [15 : 0] dread_addr;
	output reg [15 : 0] dread_data;
	input [15 : 0] dwrite_addr;
	input [15 : 0] dwrite_data;
	input [1:0] dwrite_en;
	input clk;

	reg [7:0] mem[ramsize - 1 : 0];

	initial $readmemh("test.vmem", mem);
		
	always @(posedge clk)
	begin
		if (dwrite_en[0])
			mem[dwrite_addr] <= dwrite_data[7:0];
		if (dwrite_en[1])
			mem[dwrite_addr + 1] <= dwrite_data[15:8];
		iread_data[7 : 0] <= mem[iread_addr];
		iread_data[15 : 8] <= mem[iread_addr + 1];
		iread_data[23 : 16] <= mem[iread_addr + 2];
		dread_data[7:0] <=
			(dread_addr == dwrite_addr && dwrite_en[0]) ? dwrite_data[7:0] :
			(dread_addr == dwrite_addr + 1 && dwrite_en[1]) ? dwrite_data[15:8] :
			mem[dread_addr];
		dread_data[15:8] <=
			(dread_addr == dwrite_addr && dwrite_en[1]) ? dwrite_data[15:8] :
			(dread_addr + 1 == dwrite_addr && dwrite_en[0]) ? dwrite_data[7:0] :
			mem[dread_addr + 1];
		iread_valid <= 1;
	end
endmodule

