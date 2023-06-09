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
    wire ramsel;
	wire [7:0] data_rdata;
	wire [7:0] code_rdata;
	wire [7:0] rdata = ramsel ? data_rdata : code_rdata;
	wire [7:0] addr;

    wire prg_din = in[0];
    wire prg_shift_clk = in[1];
    wire prg_latch = in[2];

	wire prg_write = prg_latch;

	wire [7:0] prg_wdata;
	wire [7:0] prg_addr;

    ShiftIn16 prg_port({prg_addr, prg_wdata}, prg_din, prg_shift_clk, prg_latch);

	TinyCore core(prg ? 1'b0 : clk, nreset, read, write, wdata, rdata, addr, ramsel, out, in);

	TinyRAM data_ram(
		clk,
		read,
		write,
		wdata,
		data_rdata,
		addr,
		ramsel
	);
	TinyRAM code_ram(
		clk,
		read,
        prg ? prg_write : write,
        prg ? prg_wdata : wdata,
        code_rdata,
        prg ? prg_addr : addr,
		prg ? 1'b1 : !ramsel
	);

	/*TinyRAM ram(clk,
        read,
        prg ? prg_write : write,
        prg ? prg_wdata : wdata,
        rdata,
        prg ? prg_addr : addr
    );*/
endmodule
