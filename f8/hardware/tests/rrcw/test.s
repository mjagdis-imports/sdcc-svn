.area HOME

	; Test 16-bit rotate right through carry.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0xaaaa
	tst	xl
	rrcw	y
	jrc	l1trap
	jrz	l1trap
	cpw	y, #0x5555
	jrz	l1
l1trap:
	trap
l1:

	tst	xl
	rrcw	y
	jrnc	l2trap
	jrz	l2trap
	cpw	y, #0x2aaa
	jrz	l2
l2trap:
	trap
l2:

	tstw	y
	rrcw	y
	jrc	l3trap
	jrz	l3trap
	cpw	y, #0x9555
	jrz	l3
l3trap:
	trap
l3:

	pushw	#0xaaaa
	tst	xl
	rrcw	(0, sp)
	jrc	l4trap
	jrz	l4trap
	ldw	y, (0, sp)
	cpw	y, #0x5555
	jrz	l4
l4trap:
	trap
l4:

	tst	xl
	rrcw	(0, sp)
	jrnc	l2trap
	jrz	l5trap
	ldw	y, (0, sp)
	cpw	y, #0x2aaa
	jrz	l5
l5trap:
	trap
l5:

	tstw	y
	rrcw	(0, sp)
	jrc	l6trap
	jrz	l6trap
	ldw	y, (0, sp)
	cpw	y, #0x9555
	jrz	l6
l6trap:
	trap
l6:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

