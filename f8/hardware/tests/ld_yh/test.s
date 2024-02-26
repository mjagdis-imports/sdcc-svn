.area HOME

	; Test 8-bit register-from-memory loads

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	; no prefix
	clrw	x
	clrw	y
	clrw	z
	ld	yh, #0x55
	cpw	x, #0x0000
	jrnz	l1trap
	cpw	y, #0x5500
	jrnz	l1trap
	cpw	z, #0x0000
	jrz	l1
l1trap:
	trap
l1:

	; altacc' prefix
	clrw	x
	clrw	y
	clrw	z
	ld	zh, #0x55
	cpw	x, #0x0000
	jrnz	l2trap
	cpw	y, #0x0000
	jrnz	l2trap
	cpw	z, #0x5500
	jrz	l2
l2trap:
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

