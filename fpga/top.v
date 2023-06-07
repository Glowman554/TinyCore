module TinySOC (
    input nreset,
    input clk,


    input [7:0] in,
	output [7:0] out,
    input prg
);
    wire read;
	wire write;

	wire [7:0] wdata;
	wire [7:0] rdata;
	wire [7:0] addr;

    wire prg_din = in[0];
    wire prg_shift_clk = in[1];
    wire prg_latch = in[2];

	wire prg_write = prg_latch;

	wire [7:0] prg_wdata;
	wire [7:0] prg_addr;

    ShiftIn16 prg_port({prg_addr, prg_wdata}, prg_din, prg_shift_clk, prg_latch);

	TinyCore core(prg ? 1'b0 : clk, nreset, read, write, wdata, rdata, addr, out, in);

	TinyRAM ram(clk,
        read,
        prg ? prg_write : write,
        prg ? prg_wdata : wdata,
        rdata,
        prg ? prg_addr : addr
    );
endmodule
