.area HOME

	; Test 16-bit loads into memory.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y
	addw	sp, #-6

	ldw	y, #0xaa55
	ldw	x, #0xa55a
	ldw	z, #0xa33a
	ldw	0x3ffe, y
	ldw	(0, sp), z
	ldw	z, #0x3f00
	ldw	(0xfc, z), x
	ldw	y, 0x3ffa
	cpw	y, #0xa33a
	jrnz	l1trap
	ldw	y, 0x3ffc
	cpw	y, #0xa55a
	jrnz	l1trap
	ldw	y, 0x3ffe
	cpw	y, #0xaa55
	jrz	l1
l1trap:
	trap
l1:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

