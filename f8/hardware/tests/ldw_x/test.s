.area HOME

	; Test 16-bit loads into accumulator

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	pushw	#0x5445
	pushw	#0x6446

	; ldw	x, (y)
	ldw	y, #0x3ffe
	ldw	x, (y)
	cpw	x, #0x5445
	jrnz	l1trap
	ldw	y, #0x3ffc
	ldw	x, (y)
	cpw	x, #0x6446
	jrnz	l1trap
	ldw	z, (y)
	cpw	z, #0x6446
	jrz	l1
l1trap:
	trap
l1:

	clrw	y
	ldw	z, #0x3ffe
	ldw	y, (z)
	cpw	y, #0x5445
	jrnz	l2trap
	ldw	x, #0x3ffc
	ldw	z, (x)
	cpw	z, #0x6446
	jrz	l2
l2trap:
	trap
l2:


	ldw	y, #0xaa55
	pushw	#0xa55a
	clrw	y
	tstw	y	; set z flag
	ldw	y, sp
	ldw	x, (y)
	jrz	l3trap
	cpw	x, #0xa55a
	jrnz	l3trap
	clrw	(0, sp)
	ldw	y, sp
	ldw	x, (y)
	jrnz	l3trap
	tstw	x
	jrz	l3
l3trap:
	trap
l3:



loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

