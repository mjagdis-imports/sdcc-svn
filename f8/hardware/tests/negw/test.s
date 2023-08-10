.area HOME

	; Test 16-bit negation.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0
	negw	y
	jrnc	l1trap
	jrnz	l1trap
	jrn	l1trap
	jro	l1trap
	cpw	y, #0
	jrz	l1
l1trap:
	trap
l1:

	ldw	y, #1
	negw	y
	jrc	l2trap
	jrz	l2trap
	jrnn	l2trap
	jro	l2trap
	cpw	y, #-1
	jrz	l2
l2trap:
	trap
l2:

	ldw	x, #0x8000
	negw	x
	jrc	l3trap
	jrz	l3trap
	jrnn	l3trap
	jrno	l3trap
	cpw	x, #0x8000
	jrz	l3
l3trap:
	trap
l3:

	ldw	z, #0
	negw	z
	jrnc	l4trap
	jrnz	l4trap
	jrn	l4trap
	jro	l4trap
	cpw	z, #0
	jrz	l1
l4trap:
	trap
l4:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

