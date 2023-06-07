module TinyALU (
	input [7:0] a,
	input [7:0] b,

	output [7:0] addition,
	output [7:0] substraction,
	output [7:0] not_and
);
	assign addition = a + b;
	assign substraction = a - b;
	assign not_and = ~(a & b);
endmodule