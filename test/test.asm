entry:
	ldi r0, 0xff
	out r0
	ldi r1, 0x00
	stmi r1, 0x01
	ji addr(entry)