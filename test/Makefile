%.hex: %.asm
	@echo AS $<
	@../assembler/assembler.elf $< $@

all: $(patsubst %.asm,%.hex,$(wildcard *.asm))

clean:
	rm *.hex -v