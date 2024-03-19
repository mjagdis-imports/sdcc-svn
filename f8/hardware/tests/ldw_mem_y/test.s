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

	; altacc', altacc''.
	pushw	#0xa55a
	ldw	z, sp
	ldw	y, #0xb66b
	ldw	(z), y
	cpw	y, #0xb66b
	jrnz	l2trap
	xchw	y, (0, sp)
	cpw	y, #0xb66b
	jrnz	l2trap
	ldw	x, sp
	ldw	z, #0x0ff0
	ldw	(x), z
	cpw	y, #0xb66b
	jrnz	l2trap
	cpw	z, #0x0ff0
	jrnz	l2trap
	popw	y
	cpw	y, #0x0ff0
	jrz	l2
l2trap:
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

