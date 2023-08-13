.area HOME

	; Test xch xl, (y)

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	push	#0xa5
	ld	xl, #0x5a
	ldw	y, #0x3fff
	xch	xl, (y)
	cp	xl, #0xa5
	jrnz	l1trap
	pop	xl
	cp	xl, #0x5a
	jrz	l1
l1trap:
	trap
l1:

	pushw	#0x00aa
	ld	xh, #0x55
	ldw	y, #0x3ffe
	xch	f, (1, sp)
	xch	xh, (y)
	xch	f, (1, sp)
	cpw	x, #0xaa5a
	jrnz	l2trap
	pop	xl
	cp	xl, #0x55
	jrnz	l2trap
	pop	xl
	and	xl, #0x1f	; mask out hidden flag bits
	jrz	l2
l2trap:
	trap
l2:

	pushw	#0x1f55
	ld	yl, #0xa5
	ldw	z, #0x3ffe
	xch	f, (1, sp)
	xch	yl, (z)
	xch	f, (1, sp)
	cp	yl, #0x55
	jrnz	l3trap
	pop	xl
	cp	xl, #0xa5
	jrnz	l3trap
	pop	xl
	and	xl, #0x1f
	cp	xl, #0x1f
	jrz	l3
l3trap:
	trap
l3:

	push	#0x34
	ld	zl, #0x55
	ldw	x, #0x3fff
	xch	zl, (x)
	cp	zl, #0x34
	jrnz	l4trap
	pop	xl
	cp	xl, #0x55
	jrz	l4
l4trap:
	trap
l4:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

