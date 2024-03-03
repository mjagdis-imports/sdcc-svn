.area HOME

	; Test 8-bit bitwise and.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	x, #0xf0a5
	ldw	y, #0x0e01
	ldw	z, #0x0204
	xor	xl, xh
	jrz	l1trap
	jrn	l1trap
	cpw	x, #0xf055
	jrnz	l1trap
	cpw	y, #0x0e01
	jrnz	l1trap
	cpw	z, #0x0204
	jrz	l1
l1trap:
	trap
l1:

	xor	xl, yl
	jrz	l2trap
	jrn	l2trap
	cpw	x, #0xf054
	jrnz	l2trap
	xor	xl, yh
	jrz	l2trap
	jrn	l2trap
	cpw	x, #0xf05a
	jrz	l2
l2trap:
	trap
l2:

	xor	xh, xl
	jrz	l3trap
	jrnn	l3trap
	cpw	x, #0xaa5a
	jrnz	l3trap
	xor	xl, #0xf5
	jrz	l3trap
	jrnn	l3trap
	cpw	x, #0xaaaf
	jrnz	l3trap
	xor	yl, #0x55
	jrz	l3trap
	jrn	l3trap
	cpw	y, #0x0e54
	jrnz	l3trap
	xor	zl, #0x04
	jrnz	l3trap
	jrn	l3trap
	cpw	z, #0x0200
	jrz	l3
l3trap:
	trap
l3:

	push	#0x01
	xor	xl, (0, sp)
	jrz	l4trap
	jrnn	l4trap
	cpw	x, #0xaaae
	jrnz	l4trap
	xor	(0, sp), xl
	jrz	l4trap
	jrnn	l4trap
	ld	xl, (0, sp)
	cpw	x, #0xaaaf
	jrnz	l4trap
	cpw	y, #0x0e54
	jrnz	l4trap
	cpw	z, #0x0200
	jrnz	l4trap
	ldw	z, #0x3000
	push	#0xf0
	xor	xl, (0xffe, z)
	jrz	l4trap
	jrn	l4trap
	cpw	x, #0xaa5f
	jrz	l4
l4trap:
	trap
l4:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

