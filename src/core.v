module TinyCore (
	input clk,
	input nreset,

	output read,
	output write,

	output [7:0] wdata,
	input [7:0] rdata,
	output [7:0] addr,

	output reg [7:0] out,
	input [7:0] in
);

	wire addr_mode = opcode == INSTRUCTION_LDM | opcode == INSTRUCTION_STM; // 1 == addr from reg, 0 == addr from im

	assign addr = (state == STATE_FETCH0 | state == STATE_FETCH1) ? program_counter : addr_mode ? registers[ir1] : instruction_im;

	assign write = state == STATE_EXECUTE & (opcode == INSTRUCTION_STM | opcode == INSTRUCTION_STMI);
	assign read = !write;

	assign wdata = registers[ir0];

	reg [7:0] instruction;
	reg [7:0] instruction_im;

	wire [3:0] opcode = instruction[7:4];
	wire [1:0] ir0 = instruction[3:2];
	wire [1:0] ir1 = instruction[1:0];

	localparam STATE_FETCH0 = 0;
	localparam STATE_FETCH1 = 1;
	localparam STATE_EXECUTE = 2;
	localparam STATE_HALT = 3;

// Auto generated. DO NOT CHANGE
// ---------------------------------------
localparam INSTRUCTION_ADDI = 4'b0; // 0x0
localparam INSTRUCTION_ADD = 4'b1; // 0x1
localparam INSTRUCTION_SUBI = 4'b10; // 0x2
localparam INSTRUCTION_SUB = 4'b11; // 0x3
localparam INSTRUCTION_NAND = 4'b100; // 0x4
localparam INSTRUCTION_LDI = 4'b101; // 0x5
localparam INSTRUCTION_MOV = 4'b110; // 0x6
localparam INSTRUCTION_LDM = 4'b111; // 0x7
localparam INSTRUCTION_LDMI = 4'b1000; // 0x8
localparam INSTRUCTION_STM = 4'b1001; // 0x9
localparam INSTRUCTION_STMI = 4'b1010; // 0xa
localparam INSTRUCTION_JZI = 4'b1011; // 0xb
localparam INSTRUCTION_JI = 4'b1100; // 0xc
localparam INSTRUCTION_OUT = 4'b1101; // 0xd
localparam INSTRUCTION_IN = 4'b1110; // 0xe
localparam INSTRUCTION_HALT = 4'b1111; // 0xf
// ---------------------------------------

	reg [7:0] program_counter;
	reg [1:0] state;

	reg [7:0] registers [0:3];

	wire [7:0] addition;
	wire [7:0] substraction;
	wire [7:0] not_and;
	wire alu_mode = opcode == INSTRUCTION_SUBI | opcode == INSTRUCTION_ADDI; // 0 == b from reg, 1 == b from im
	TinyALU alu(registers[ir0], alu_mode ? instruction_im : registers[ir1], addition, substraction, not_and);

	always @(negedge clk) begin
		if (nreset) begin
			case (state)
				STATE_FETCH0:
					begin
						instruction <= rdata;
						program_counter <= program_counter + 1;
						state <= STATE_FETCH1;
					end
				STATE_FETCH1:
					begin
						instruction_im <= rdata;
						program_counter <= program_counter + 1;
						state <= STATE_EXECUTE;
					end
				STATE_EXECUTE:
					begin
						case (opcode)
							// INSTRUCTION_NOP
							INSTRUCTION_ADDI:
								begin
									registers[ir0] <= addition;
								end
							INSTRUCTION_ADD:
								begin
									registers[ir0] <= addition;
								end
							INSTRUCTION_SUBI:
								begin
									registers[ir0] <= substraction;
								end
							INSTRUCTION_SUB:
								begin
									registers[ir0] <= substraction;
								end
							INSTRUCTION_NAND:
								begin
									registers[ir0] <= not_and;
								end
							INSTRUCTION_LDI:
								begin
									registers[ir0] <= instruction_im;
								end
							INSTRUCTION_MOV:
								begin
									registers[ir0] <= registers[ir1];
								end
							INSTRUCTION_LDM:
								begin
									registers[ir0] <= rdata;
								end
							INSTRUCTION_LDMI:
								begin
									registers[ir0] <= rdata;
								end
							INSTRUCTION_JZI:
								begin
									if (registers[ir0] == 0) begin
										program_counter <= instruction_im;
									end
								end
							INSTRUCTION_JI:
								begin
									program_counter <= instruction_im;
								end
							INSTRUCTION_OUT:
								begin
									out <= registers[ir0];
								end
							INSTRUCTION_IN:
								begin
									registers[ir0] <= in;
								end
						endcase
						if (opcode == INSTRUCTION_HALT) begin
								state <= STATE_HALT;
							end else begin 
							state <= STATE_FETCH0;
							end
						end
				endcase
		end else begin
			// reset
			program_counter <= 8'b00000000; // reset address
			state <= STATE_FETCH0;
			out <= 0;
			instruction <= 0;
			instruction_im <= 0;
		end
	end
endmodule