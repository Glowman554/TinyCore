module ShiftOut (
	input [7:0] data,
	input clk,
	input nreset,

	output dout,
	output shift_clk,
	output latch
);
	
	reg [3:0] sm = 0;

    assign latch = sm[3];

	assign shift_clk = clk & (~latch);
	assign dout = data[sm[2:0]] & (~latch);

	always @(negedge clk) begin
		if (nreset == 1'b0) begin
			sm <= 0;
		end else begin
			sm <= sm + 1;
		end
	end
endmodule

module ShiftIn16 (
    output reg [15:0] data,

    input din,
    input shift_clk,
    input latch
);

    reg [15:0] data_buffer;

    always @(posedge shift_clk) begin 
            data_buffer <= (data_buffer << 1) | din;
    end

    always @(posedge latch) begin 
            data <= data_buffer;
    end
endmodule