`begin_keywords "1800-2009"

// Dual port RAM module suitable for iCE40 4kb Embedded Block RAM
module dualportram(din, write_en, waddr, raddr, dout, clk);
	parameter ADDRBITS = 9; // 512 x
	parameter DATAWIDTH = 8; // x 8
	input write_en, clk;
	input [ADDRBITS-1:0] waddr, raddr;
	input [DATAWIDTH-1:0] din;
	output reg [DATAWIDTH-1:0] dout;

	reg [DATAWIDTH-1:0] mem[(1 << ADDRBITS) - 1 : 0];
  
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

// ADDRBITS = 10: 1K RAM at 0x3c00 to 0x3fff
// ADDRBITS = 11: 2K RAM at 0x3800 to 0x3fff
// ADDRBITS = 12: 4K RAM at 0x3000 to 0x3fff
// ADDRBITS = 13: 8K RAM at 0x2000 to 0x3fff
module ram #(parameter ADDRBITS = 10) (input logic [15:0] dread_addr, output logic [15:0] dread_data, input logic [15:0] dwrite_addr, input logic [15:0] dwrite_data, input logic[1:0] dwrite_en, input logic clk);
	localparam SIZE = 1 << (ADDRBITS - 1);
	localparam logic [15:0] RAMBASE = 16'h4000 - SIZE;

	logic [ADDRBITS-1:0] dwrite_addr_rambased, dwrite_addr_rambased_even, dwrite_addr_rambased_odd;
	logic [15:0] dread_addr_rambased, dread_addr_rambased_even, dread_addr_rambased_odd;
	wire [15:0] dwrite_addr_even, dwrite_addr_odd;
	wire [15:0] dread_addr_even, dread_addr_odd;
	wire dwrite_en_even, dwrite_en_odd;
	wire [7:0] dwrite_data_even, dread_ram_data_even;
	wire [7:0] dwrite_data_odd, dread_ram_data_odd;

	assign dread_addr_even = dread_addr[0] ? dread_addr + 1 : dread_addr;
	assign dread_addr_odd = dread_addr[0] ? dread_addr : dread_addr + 1;
	assign dwrite_addr_even = dwrite_addr[0] ? dwrite_addr + 1 : dwrite_addr;
	assign dwrite_addr_odd = dwrite_addr[0] ? dwrite_addr : dwrite_addr + 1;
	assign dwrite_en_even = (dwrite_addr[0] ? dwrite_en[1] : dwrite_en[0]);
	assign dwrite_en_odd = (dwrite_addr[0] ? dwrite_en[0] : dwrite_en[1]);
	assign dwrite_data_even = dwrite_addr[0] ? dwrite_data[15:8] : dwrite_data[7:0];
	assign dwrite_data_odd = dwrite_addr[0] ? dwrite_data[7:0] : dwrite_data[15:8];

	assign dwrite_addr_rambased_even = dwrite_addr_even - RAMBASE;
	assign dwrite_addr_rambased_odd = dwrite_addr_odd - RAMBASE;
	assign dread_addr_rambased_even = dread_addr_even - RAMBASE;
	assign dread_addr_rambased_odd = dread_addr_odd - RAMBASE;

	dualportram #(.ADDRBITS(ADDRBITS-1)) evenram
		(.din(dwrite_data_even), .write_en(dwrite_en_even), .waddr(dwrite_addr_even[ADDRBITS-1:1]),
		.raddr(dread_addr_even[ADDRBITS-1:1]), .dout(dread_ram_data_even),
		.clk(clk));
	dualportram #(.ADDRBITS(ADDRBITS-1)) oddram
		(.din(dwrite_data_odd), .write_en(dwrite_en_odd), .waddr(dwrite_addr_odd[ADDRBITS-1:1]),
		.raddr(dread_addr_odd[ADDRBITS-1:1]), .dout (dread_ram_data_odd),
		.clk(clk));

	logic dread_odd;
	always @(posedge clk)
	begin
		dread_odd = dread_addr[0];
	end

	assign dread_data[7:0] = dread_odd ? dread_ram_data_odd : dread_ram_data_even;
	assign dread_data[15:8] = dread_odd ? dread_ram_data_even : dread_ram_data_odd;
endmodule

`end_keywords

