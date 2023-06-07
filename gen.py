def encode_instruction(OP, IR0, IR1):
	assert OP <= 0b1111
	assert IR0 <= 0b11
	assert IR1 <= 0b11

	return (OP << 4) | (IR0 << 2) | IR1

def write_instruction(INSTR, IM, f):
	assert IM <= 0b11111111
	f.write(hex(INSTR)[2:] + " " + hex(IM)[2:] + "\n")

I = {}

with open("opcodes.txt") as f:
	for line in f:
		split = line.split(":")
		if len(split) == 2:
			I[split[1].strip().split(" ")[0]] = int(split[0][2:], 2)

print(I)

for i in I:
	print(f"localparam INSTRUCTION_{i} = 4'b{bin(I[i])[2:]}; // {hex(I[i])}")

for i in I:
	print(f"#define INSTRUCTION_{i} {I[i]}")

"""
with open("firmware.hex", "w") as f:
	f.write("@0\n")
	# write_instruction(encode_instruction(I["LDI"], 0, 0), 0x69, f)
	# write_instruction(encode_instruction(I["LDI"], 1, 0), 0x6a, f)

	# write_instruction(encode_instruction(I["LDMI"], 2, 0), 0x00, f)

	# write_instruction(encode_instruction(I["LDI"], 2, 0), 0x02, f)
	# write_instruction(encode_instruction(I["LDM"], 3, 2), 0x00, f)

	# write_instruction(encode_instruction(I["STMI"], 3, 0), 0x03, f)

	# write_instruction(encode_instruction(I["JI"], 0, 0), 0x02, f)

	# write_instruction(encode_instruction(I["HALT"], 0, 0), 0, f)

	write_instruction(encode_instruction(I["LDI"], 0, 0), 0, f)
	write_instruction(encode_instruction(I["LDI"], 1, 0), 1, f)
	write_instruction(encode_instruction(I["LDI"], 2, 0), 11, f)

	# LOOP BEGIN @ 3 * 2

	write_instruction(encode_instruction(I["MOV"], 3, 0), 0, f)
	write_instruction(encode_instruction(I["ADD"], 3, 1), 0, f)
	write_instruction(encode_instruction(I["MOV"], 0, 1), 0, f)
	write_instruction(encode_instruction(I["MOV"], 1, 3), 0, f)
	write_instruction(encode_instruction(I["SUBI"], 2, 0), 1, f)
	write_instruction(encode_instruction(I["JZI"], 2, 0), 10 * 2, f)
	write_instruction(encode_instruction(I["JI"], 2, 0), 3 * 2, f)

	write_instruction(encode_instruction(I["OUT"], 3, 0), 0, f)
	
	write_instruction(encode_instruction(I["HALT"], 0, 0), 0, f)
"""
