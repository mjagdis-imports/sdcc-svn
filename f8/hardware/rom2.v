`begin_keywords "1800-2009"

// ROM
module rom #(parameter SIZE = 2048,  logic [15:0] ROMBASE = 16'h4000)
	(input logic [14:0] dread_addr_even, output logic [7:0] dread_data_even,
	input logic [14:0] dread_addr_odd, output logic [7:0] dread_data_odd,
	input logic clk);

	(* ram_style = "block" *) reg [7:0] rom[SIZE - 1 : 0];

	initial $readmemh("test.vmem", rom);

	always @(posedge clk)
	begin
		dread_data_even <= rom[{dread_addr_even, 1'b0}];
		dread_data_odd <= rom[{dread_addr_odd, 1'b1}];
	end
endmodule

`end_keywords

