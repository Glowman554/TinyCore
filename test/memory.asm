entry:
	ldi r0, 0x20 ; memory address
	ldi r1, 0x69 ; magic value

	stm r1, r0
	ldm r2, r0

	subi r2, 0x69
	jzi r2, addr(success)

	halt

success:
	ldi r0, 0xff
	out r0
	halt