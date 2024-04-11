`begin_keywords "1800-2009"

// ROM
module rom #(parameter SIZE = 2048,  logic [15:0] ROMBASE = 16'h4000)
	(input logic [14:0] read_addr_even, output logic [7:0] read_data_even,
	input logic [14:0] read_addr_odd, output logic [7:0] read_data_odd,
	input logic clk);

	(* ram_style = "block" *) reg [7:0] rom[SIZE - 1 : 0];

	initial $readmemh("test.vmem", rom);

	logic [15:0] read_addr_even_rombased, read_addr_odd_rombased;
	assign read_addr_even_rombased = {read_addr_even, 1'b0} - ROMBASE;
	assign read_addr_odd_rombased = {read_addr_odd, 1'b1} - ROMBASE;

	always_ff @(posedge clk)
	begin
		read_data_even <= rom[read_addr_even_rombased];
		read_data_odd <= rom[read_addr_odd_rombased];
	end
endmodule

`end_keywords

