.area HOME

	; Test 16-bit logic left shift

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	push	#0x00
	ldw	y, #0xaa55
	xch	f, (0, sp)
	sllw	y
	xch	f, (0, sp)
	cpw	y, #0x54aa
	jrnz	l1trap
	ld	xl, (0, sp)
	and	xl, #0x1f	; mask hidden flags
	cp	xl, #0x02
	jrnz	l1trap
	clr	(0, sp)
	ldw	y, #0
	xch	f, (0, sp)
	sllw	y
	xch	f, (0, sp)
	cpw	y, #0
	jrnz	l1trap
	ld	xl, (0, sp)
	and	xl, #0x1f
	cp	xl, #0x08
	jrz	l1
l1trap:
	trap
l1:

	push	#0x00
	ldw	z, #0xaa55
	xch	f, (0, sp)
	sllw	z
	xch	f, (0, sp)
	cpw	z, #0x54aa
	jrnz	l2trap
	ld	xl, (0, sp)
	and	xl, #0x1f	; mask hidden flags
	cp	xl, #0x02
	jrnz	l2trap
	clr	(0, sp)
	ldw	x, #0
	xch	f, (0, sp)
	sllw	x
	xch	f, (0, sp)
	cpw	x, #0
	jrnz	l2trap
	ld	xl, (0, sp)
	and	xl, #0x1f
	cp	xl, #0x08
	jrz	l2
l2trap:
	trap
l2:

	ldw	y, #0x5a5a
	ld	xl, #4
	sllw	y, xl
	jrz	l3trap
	cpw	y, #0xa5a0
	jrnz	l3trap
	ld	xl, #12
	sllw	y, xl
	jrnz	l3trap
	cpw	y, #0x0000
	jrnz	l3trap
	ldw	y, #0x0011
	ld	xh, #1
	sllw	y, xh
	cpw	y, #0x0022
	jrnz	l3trap
	ld	xh, #7
	sllw	y, xh
	cpw	y, #0x1100
	jrnz	l3trap
	ldw	x, #0xaa55
	ld	zl, #4
	sllw	x, zl
	cpw	x, #0xa550
	jrnz	l3trap
	ldw	z, #0x0380
	ld	yl, #1
	sllw	z, yl
	cpw	z, #0x0700
	jrnz	l3trap
	ldw	y, #0xa5a5
	ld	zh, #0x1
	sllw	y, zh
	jrz	l3trap
	jrn	l3trap
	cpw	y, #0x4b4a
	jrnz	l3trap
	sllw	y, zh
	jrz	l3trap
	jrnn	l3trap
	cpw	y, #0x9694
	jrnz	l3trap
	ld	zh, #14
	sllw	y, zh
	jrnz	l3trap
	jrnn	l3
l3trap:
	trap
l3:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

