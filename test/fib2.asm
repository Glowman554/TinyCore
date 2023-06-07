entry:
	ldi r0, 0
	ldi r1, 1
	ldi r2, 11

loop:
	mov r3, r0
	add r3, r1
	mov r0, r1
	mov r1, r3
	subi r2, 1
	jzi r2, addr(out)
	ji addr(loop)
out:
	out r3
	halt