module sim;
	reg 	 clk = 0;
	reg nreset = 0;
	always #2 clk = !clk;

	
	wire read;
	wire write;

	wire [7:0] wdata;
	wire [7:0] rdata;
	wire [7:0] addr;

	wire [7:0] out;
	reg [7:0] in = 8'haa;

	TinyCore core(clk, nreset, read, write, wdata, rdata, addr, out, in);
	TinyRAM ram(clk, read, write, wdata, rdata, addr);

	initial begin
        $dumpfile("dump.lxt");
        $dumpvars(0, sim);

		$monitor("r0: %h, r1: %h, r2: %h, r3: %h, out: %h", core.registers[0], core.registers[1], core.registers[2], core.registers[3], out);

        #11 nreset = 1;
		
        #4000 $finish;
    end
endmodule