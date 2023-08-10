.area HOME

	; Test swapping of bytes in word (xch yl, yh)

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	x, #0xa55a
	ldw	y, #0xaa55
	ldw	z, #0x3bb3
	push	#0x00
	xch	f, (0, sp)
	xch	yl, yh
	xch	xl, xh
	xch	zl, zh
	xch	f, (0, sp)
	cpw	x, #0x5aa5
	jrnz	l1trap
	cpw	y, #0x55aa
	jrnz	l1trap
	cpw	z, #0xb33b
	jrnz	l1trap
	push	#0x1f
	xch	f, (0, sp)
	xch	xl, xh
	xch	f, (0, sp)
	cpw	x, #0xa55a
	jrnz	l1trap
	xch	yl, yh
	cpw	y, #0xaa55
	jrnz	l1trap
	xch	zl, zh
	cpw	z, #0x3bb3
	jrnz	l1trap
	ldw	x, (0, sp)	; get flags from stack.
	and	xl, #0x1f	; mask outhidden flag bits
	and	xh, #0x1f	; mask outhidden flag bits
	cpw	x,  #0x001f
	jrz	l1
l1trap:
	trap
l1:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

