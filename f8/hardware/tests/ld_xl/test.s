.area HOME

	; Test 8-bit register-from-memory loads

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	; no prefix
	pushw	#0x55aa
	ldw	x, #0x3333
	ld	xl, #0xa5
	cpw	x, #0x33a5
	jrnz	l1trap
	ld	xl, 0x3ffe
	cpw	x, #0x33aa
	jrnz	l1trap
	ld	xl, (1, sp)
	cpw	x, #0x3355
	jrnz	l1trap
	ldw	z, #0x3000
	ld	xl, (0xffe, z)
	cpw	x, #0x33aa
	jrz	l1
l1trap:
	trap
l1:

	; altacc prefix
	ldw	x, #0x3333
	ld	xh, #0xa5
	cpw	x, #0xa533
	jrnz	l2trap
	ld	xh, 0x3ffe
	cpw	x, #0xaa33
	jrnz	l2trap
	ld	xh, (1, sp)
	cpw	x, #0x5533
	jrnz	l2trap
	ldw	z, #0x3000
	ld	xh, (0xffe, z)
	cpw	x, #0xaa33
	jrz	l2
l2trap:
	trap
l2:

	; altacc' prefix
	ldw	x, #0x3330
	ldw	y, #0x4440
	ldw	z, #0x5550
	ld	yl, #0xa5
	cpw	y, #0x44a5
	jrnz	l3trap
	ld	yl, 0x3ffe
	cpw	y, #0x44aa
	jrnz	l3trap
	ld	yl, (1, sp)
	cpw	y, #0x4455
	jrnz	l3trap
	ldw	z, #0x3000
	ld	yl, (0xffe, z)
	cpw	y, #0x44aa
	jrz	l3
l3trap:
	trap
l3:

	; altacc'' prefix
	ldw	z, #0x5555
	ld	zl, #0x5a
	cpw	z, #0x555a
	jrnz	l4trap
	ld	zl, 0x3fff
	cpw	z, #0x5555
	jrnz	l4trap
	ld	zl, (0, sp)
	cpw	z, #0x55aa
	jrnz	l4trap
	ldw	z, #0x3000
	ld	zl, (0xfff, z)
	cpw	z, #0x3055
	jrz	l4
l4trap:
	trap
l4:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

