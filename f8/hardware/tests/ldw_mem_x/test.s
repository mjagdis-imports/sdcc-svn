.area HOME

	; Test 16-bit loads into memory.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	addw	sp, #-4
	ldw	y, #0x3ffc
	ldw	x, #0xa55a
	ldw	(y), x
	ldw	x, #0xa33a
	ldw	(2, y), x
	popw	y
	cpw	y, #0xa55a
	jrnz	l1trap
	popw	y
	cpw	y, #0xa33a
	jrz	l1
l1trap:
	trap
l1:

	addw	sp, #-4
	ldw	z, #0x3ffc
	ldw	x, #0xa66a
	ldw	(0, z), x
	ldw	x, #0xa44a
	ldw	(2, z), x
	popw	y
	cpw	y, #0xa66a
	jrnz	l2trap
	popw	y
	cpw	y, #0xa44a
	jrz	l2
l2trap:
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

