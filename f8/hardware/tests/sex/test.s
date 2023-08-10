.area HOME

	; Test 8-to-16 bit sign extension

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	x, #0x5555
	ldw	z, #0x5555
	ldw	y, #0x5555
	ld	xl, #0x7f
	sex	y, xl
	jrz	l1trap
	jrn	l1trap
	cpw	y, #0x007f
	jrnz	l1trap
	ld	xl, #0x80
	sex	y, xl
	jrz	l1trap
	jrnn	l1trap
	cpw	y, #0xff80
	jrnz	l1trap
	ld	xl, #0x00
	sex	y, xl
	jrnz	l1trap
	jrn	l1trap
	cpw	y, #0x0000
	jrz	l1
l1trap:
	trap
l1:

	ld	xh, #0xff
	sex	y, xh
	cpw	y, #0xffff
	jrnz	l2trap
	ld	yl, #0x8a
	sex	z, yl
	cpw	z, #0xff8a
	jrnz	l2trap
	ld	zl, #0xa5
	sex	x, zl
	cpw	x, #0xffa5
	jrz	l2
l2trap:
	trap
l2:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

