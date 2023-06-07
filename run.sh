set -e

for file in $(find "test" -type f -name "*.hex"); do
    # Process the .hex file here
    echo "Running $file..."
	iverilog -o sim -DFIRMWARE=\"$file\" src/*.v sim.v
	vvp sim -lxt2
	echo
done

# iverilog -o sim core.v alu.v sim.v
# vvp sim -lxt2

# # gtkwave --save dump.gtkw dump.lxt