.area HOME

	; Test 8-to-16 bit zero-extension

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0xffff
	ldw	z, #0xffff
	ld	xl, #0xa5
	zex	y, xl
	jrz	l1trap
	cpw	y, #0x00a5
	jrnz	l1trap
	clr	xl
	zex	y, xl
	jrnz	l1trap
	cpw	y, #0x0000
	jrnz	l1trap
	ld	xh, #0x5a
	zex	y, xh
	cpw	y, #0x005a
	jrnz	l1trap
	zex	z, yl
	cpw	z, #0x005a
	jrnz	l1trap
	ld	zl, #0xa0
	zex	x, zl
	jrz	l1trap
	cpw	x, #0x00a0
	jrz	l1
l1trap:
	trap
l1:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

