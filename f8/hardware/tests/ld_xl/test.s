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

	; unprefixed loads from (y) and (n, y)
	pushw	#0xa55a
	ldw	y, sp
	ld	xl, (y)
	cp	xl, #0x5a
	jrnz	l5trap
	ld	xl, (1, y)
	cp	xl, #0xa5
	jrz	l5
l5trap:
	trap
l5:

	; altacc loads from (y) and (n, y)
	ld	xh, (y)
	cp	xh, #0x5a
	jrnz	l6trap
	ld	xh, (1, y)
	cp	xh, #0xa5
	jrz	l6
l6trap:
	trap
l6:

	; altacc'' loads from (y) and (n, y)
	ld	zl, (y)
	cp	zl, #0x5a
	jrnz	l7trap
	ld	zl, (1, y)
	cp	zl, #0xa5
	jrz	l7
l7trap:
	trap
l7:

	; altacc' loads from (y) and (n, y)
	pushw	#0xa55a
	ldw	y, sp
	ld	yl, (y)
	cp	yl, #0x5a
	jrnz	l8trap
	ldw	y, sp
	ld	yl, (1, y)
	cp	yl, #0xa5
	jrz	l8
l8trap:
	trap
l8:

	; Load from memory updates n and z, but neither o nor c.
	ld	xl, #0x00
	tst	xl
	sll	xl
	push	#0x80
	ld	xl, (0, sp)
	jrz	l9trap
	jrnn	l9trap
	jrc	l9trap
	jro	l9trap
	push	#0x00
	ld	xl, #0x83
	tst	xl
	sll	xl
	ld	xl, (0, sp)
	jrnz	l9trap
	jrn	l9trap
	jrnc	l9trap
	jro	l9
l9trap:
	trap
l9:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

