.area HOME

	; Test 8-bit pop

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	pushw	#0xaa55
	pop	xl
	cp	xl, #0x55
	jrnz	l1trap
	ldw	y, sp
	cpw	y, #0x3fff
	jrnz	l1trap
	pop	xl
	cp	xl, #0xaa
	jrnz	l1trap
	ldw	y, sp
	cpw	y, #0x4000
	jrz	#l1
l1trap:
	trap
l1:

	push	#0x00
	pushw	#0xa55a
	xch	f, (2, sp)
	pop	xh
	xch	f, (1, sp)
	cp	xh, #0x5a
	jrnz	l2trap
	ld	xl, (1, sp)
	and	xl, #0x1f
	cp	xl, #0x00
	jrnz	l2trap
	ld	xl, #0x1f
	ld	(1, sp), xl
	xch	f, (1, sp)
	pop	yl
	xch	f, (0, sp)
	cp	yl, #0xa5
	jrnz	l2trap
	ld	xl, (0, sp)
	and	xl, #0x1f
	cp	xl, #0x1f
	jrz	#l2
l2trap:
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

