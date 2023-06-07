entry:
	ldi r0, 0
	ldi r1, 0xff

loop:
	out r0
	out r1
	ji addr(loop)