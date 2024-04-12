`begin_keywords "1800-2009"

// ROM
module rom #(parameter SIZE = 2048,  logic [15:0] ROMBASE = 16'h4000)
	(input logic [14:0] read_addr_even, output logic [7:0] read_data_even,
	input logic [14:0] read_addr_odd, output logic [7:0] read_data_odd,
	input logic clk);

	(* ram_style = "block" *) reg [7:0] evenrom[SIZE / 2 - 1 : 0];
	(* ram_style = "block" *) reg [7:0] oddrom[SIZE / 2 - 1 : 0];

	initial $readmemh("test.even.vmem", evenrom);
	initial $readmemh("test.odd.vmem", oddrom);

	logic [14:0] read_addr_even_rombased, read_addr_odd_rombased;
	assign read_addr_even_rombased = read_addr_even - ROMBASE / 2;
	assign read_addr_odd_rombased = read_addr_odd - ROMBASE / 2;

	always_ff @(posedge clk)
	begin
		read_data_even <= evenrom[read_addr_even_rombased];
		read_data_odd <= oddrom[read_addr_odd_rombased];
	end
endmodule

`end_keywords

