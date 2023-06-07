module TinyRAM (
	input clk,

	input read,
	input write,

	input [7:0] wdata,
	output [7:0] rdata,
	input [7:0] addr
);
	reg [7:0] memory [0:255];

	assign rdata = read ? memory[addr] : 0;

	always @(posedge clk) begin
		if (write) begin
			memory[addr] <= wdata;
		end
	end

	initial begin
`ifdef FIRMWARE
		$readmemh(`FIRMWARE, memory);
`else
        // $readmemh("test/fib2.hex", memory);
`endif
    end
endmodule