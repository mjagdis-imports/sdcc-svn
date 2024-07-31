.area HOME

	; Test msk.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	push	#0xff
	ldw	y, #0x3fff
	ld	xl, #0x00
	msk	(y), xl, #0x55
	ld	xl, (0, sp)
	cp	xl, #0xaa
	jrnz	#l1trap
	ld	xl, #0x05
	msk	(y), xl, #0x0f
	ld	xl, (0, sp)
	cp	xl, #0xa5
	jrnz	#l1trap
	
	jp	#l1
l1trap:
	trap
l1:

	clr	(0, sp)
	ldw	x, #0xff00
	msk	(y), xh, #0x55
	ld	xl, (0, sp)
	cp	xl, #0x55
	jrnz	l2trap
	push	#0xff
	ldw	z, #0x3ffe
	ld	yl, #0xff
	ld	(0, sp), yl
	ld	yl, #0x00
	msk	(z), yl, #0xa5
	ld	xl, (0, sp)
	cp	xl, #0x5a
	jrnz	l2trap
	clr	(0, sp)
	ldw	x, #0x3ffe
	ld	zl, #0xff
	msk	(x), zl, #0x11
	ld	xl, (0, sp)
	cp	xl, #0x11
	jrz	l2
l2trap:
	trap
l2:

	; Check effect on flags
	clr	(0, sp)
	ld	xl, #0xff
	ldw	y, sp
	msk	(y), xl, #0x55
	jrnz	l3trap
	ld	xl, (y)
	cp	xl, #0x55
	jrnz	l3trap
	ld	xl, #0x00
	msk	(y), xl, #0x55
	jrz	l3trap
	ld	xl, (y)
	jrnz	l3trap
	ldw	z, #0x5f00
	push	#0x1f
	xch	f, (0, sp)	; Set all non-reserved flags
	msk	(y), zh, #0xf5
	xch	f, (0, sp)
	ld	xl, (y)
	cp	xl, #0x55
	jrnz	l3trap
	pop	xl
	and	xl, #0x1f
	cp	xl, #0x1f	; All non-reserved flags should be set.
	jrz	l3
l3trap:
	trap
l3:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

