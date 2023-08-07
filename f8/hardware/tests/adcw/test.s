.area HOME

	; Test 16-bit addition with carry.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0
	tst	xl	; reset carry
	adcw	y, #0
	jrc	l1trap
	jrnz	l1trap
	jrn	l1trap
	cpw	y, #0
	jrz	l1
l1trap:
	trap
l1:

	ldw	y, #0x7fff
	tst	xl	; reset carry
	adcw	y, #0x0001
	jrc	l2trap
	jrz	l2trap
	jrnn	l2trap
	cpw	y, #0x8000
	jrz	l2
l2trap:
	trap
l2:

	ldw	x, #0
	tst	xl	; reset carry
	adcw	x, #0
	jrc	l3trap
	jrnz	l3trap
	jrn	l3trap
	cpw	x, #0
	jrz	l3
l3trap:
	trap
l3:

	ldw	x, #0x7fff
	tst	xl	; reset carry
	adcw	x, #0x0001
	jrc	l4trap
	jrz	l4trap
	jrnn	l4trap
	cpw	x, #0x8000
	jrz	l4
l4trap:
	trap
l4:

	ldw	y, #0x8000
	tst	xl	; reset carry
	adcw	y, #0x8000
	jrnc	l5trap
	jrn	l5trap
	jrn	l5trap
	jr	l5
l5trap:
	trap
l5:

	ldw	z, #0x000
	adcw	z, #0xffff
	jrnc	l6trap
	jrnz	l6trap
	jrn	l6trap
	cpw	z, #0x0000
	jrz	l6
l6trap:
	trap
l6:

	pushw	#0xffff
	tst	xl	; reset carry
	ldw	y, #0x0001
	adcw	y, (0, sp)
	jrnc	l7trap
	jrnz	l7trap
	jrn	l7trap
	jr	l7
l7trap:
	trap
l7:

	ldw	x, #0x0001
	adcw	x, 0x3ffe
	jrnc	l8trap
	jrz	l8trap
	jrn	l8trap
	cpw	x, #0x0001
	jrz	l8
l8trap:
	trap
l8:

	ldw	y, #0
	tst	xl	; reset carry
	ldw	x, #0
	adcw	y, x
	jrc	l9trap
	jrnz	l9trap
	jrn	l9trap
	cpw	y, #0
	jrz	l9
l9trap:
	trap
l9:

	ldw	y, #0x7fff
	tst	xl	; reset carry
	ldw	x, #0x0001
	adcw	y, x
	jrc	latrap
	jrz	latrap
	jrnn	latrap
	cpw	y, #0x8000
	jrz	la
latrap:
	trap
la:

	tst	xl	; reset carry
	ldw	y, #0x0001
	adcw	(0, sp), y
	jrnc	lbtrap
	jrnz	lbtrap
	jrn	lbtrap
	jr	lb
lbtrap:
	trap
lb:

	ldw	y, #0x0000
	adcw	0x3ffe, y
	jrc	lctrap
	jrz	lctrap
	jrn	lctrap
	ldw	y, #0x0001
	subw	y, 0x3ffe
	jrz	lc
lctrap:
	trap
lc:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

