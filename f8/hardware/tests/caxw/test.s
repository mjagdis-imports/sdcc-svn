.area HOME

	; Test 16-bit compare-and-exchange.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	pushw	#0xa5a5
	ldw	y, #0x3ffe
	ldw	z, #0x33ee
	ldw	x,#0x5a5a
	caxw	(y), z, x
	jrz	l1trap
	cpw	z, #0xa5a5
	jrnz	l1trap
	ldw	z, #0xa5a5
	ldw	x, #0x5a5a
	caxw	(y), z, x
	jrnz	l1trap
	cpw	x, #0x5a5a
	jrnz	#l1trap
	popw	y
	cpw	y, #0x5a5a
	jrz	l1
l1trap:
	trap
l1:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

