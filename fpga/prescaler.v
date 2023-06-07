module Prescaler (
	input clk,
	input nreset,

	input [31:0] psc,

	output reg sclk
);
	reg [31:0] count;

	always @(posedge clk) begin
		if (nreset == 1'b0) begin
			count <= 4'b0;
			sclk <= 0;
		end else begin
			count <= count + 1'b1;

			if (count > psc) begin
				count <= 0;
				sclk <= ~sclk;
			end
		end
	end
endmodule 