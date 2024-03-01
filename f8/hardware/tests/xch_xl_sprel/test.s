.area HOME

	; Test swapping of bytes between register and stack (xch xl, (n, sp))

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	push	#0x55
	ldw	x, #0xa5aa
	ldw	y, #0x3b5a
	ldw	z, #0x11a5
	push	#0x00
	xch	f, (0, sp)
	xch	xl, (1, sp)
	xch	f, (0, sp)
	cp	xl, #0x55
	jrnz	l1trap
	ld	xl, (1, sp)
	cp	xl, #0xaa
	jrnz	l1trap
	ld	xl, (0, sp)
	and	xl, #0x1f
	cp	xl, #0x00
	jrnz	l1trap
	push	#0x1f
	xch	f, (0, sp)
	xch	xh, (1, sp)
	xch	f, (0, sp)
	cp	xh, #0x00
	jrnz	l1trap
	ld	xl, (1, sp)
	cp	xl, #0xa5
	jrnz	l1trap
	ld	xl, (0, sp)
	and	xl, #0x1f
	cp	xl, #0x1f
	jrnz	l1trap
	ldw	x, #0xffff
	pushw x
	xch	yl, (0, sp)
	xch	zl, (1, sp)
	cpw	y, #0x3bff
	jrnz	l1trap
	cpw	z, #0x11ff
	jrnz	l1trap
	ldw	y, (0, sp)
	cpw	y, #0xa55a
	jrz	l1
l1trap:
	trap
l1:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

