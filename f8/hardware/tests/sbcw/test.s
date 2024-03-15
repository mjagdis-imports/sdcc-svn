.area HOME

	; Test 16-bit subtraction with carry.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0x0001
	ldw	x, #0x0001
	tst	xl	; reset c
	sbcw	y, x
	jrc	l1trap
	jrz	l1trap
	jrnn	l1trap
	jro	l1trap
	cpw	y, #0xffff
	jrz	l1
l1trap:
	trap
l1:

	ldw	y, #0x0001
	ldw	x, #0x7fff
	tstw	y	; set c
	sbcw	y, x
	jrc	l2trap
	jrz	l2trap
	jrnn	l2trap
	jro	l2trap
	cpw	y, #0x8002
	jrz	l2
l2trap:
	trap
l2:

	ldw	y, #0x0100
	ldw	x, #0x0100
	tstw	y	; set c
	sbcw	y, x
	jrnc	l3trap
	jrnz	l3trap
	jrn	l3trap
	jro	l3trap
	tstw	y
	jrz	l3
l3trap:
	trap
l3:

	ldw	y, #0x0002
	pushw	#0x0001
	tst	xl	; reset c
	sbcw	y, (0, sp)
	jrnc	l4trap
	jrnz	l4trap
	jrn	l4trap
	jro	l4trap
	tstw	y
	jrz	l4
l4trap:
	trap
l4:

	ldw	y, #0x0002
	tst	xl	; reset c
	sbcw	y, 0x3ffe
	jrnc	l5trap
	jrnz	l5trap
	jrn	l5trap
	jro	l5trap
	tstw	y
	jrz	l5
l5trap:
	trap
l5:	

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

