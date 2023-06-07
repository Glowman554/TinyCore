entry:
	ldi r1, 0x69 ; magic value

	stmi r1, 0x20
	ldmi r2, 0x20

	subi r2, 0x69
	jzi r2, addr(success)

	halt

success:
	ldi r0, 0xff
	out r0
	halt