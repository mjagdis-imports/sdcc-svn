.area HOME

	; Test one-operand wide add carry.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0
	tst	xl	; reset c
	adcw	y
	jro	l1trap
	jrc	l1trap
	jrnz	l1trap
	jrn	l1trap
	cpw	y, #0
	jrz	l1
l1trap:
	trap
l1:
	tstw	y	; set c
	adcw	y
	jro	l2trap
	jrc	l2trap
	jrz	l2trap
	jrn	l2trap
	cpw	y, #1
	jrz	l2
l2trap:
	trap
l2:

	addw	y, #0x00fe
	tstw	y
	adcw	y
	jro	l3trap
	jrc	l3trap
	jrz	l3trap
	jrn	l3trap
	cpw	y, #0x0100
	jrz	l3
l3trap:
	trap
l3:

	addw	y, #0xfeff
	tst	xl
	adcw	y
	jro	l4trap
	jrc	l4trap
	jrz	l4trap
	jrnn	l4trap
	cpw	y, #0xffff
	jrz	l4
l4trap:
	trap
l4:
	tstw	y
	adcw	y
	jro	l5trap
	jrnc	l5trap
	jrnz	l5trap
	jrn	l5trap
	cpw	y, #0x0000
	jrz	l5
l5trap:
	trap
l5:

	pushw	#0x7fff
	tstw	y
	adcw	(0, sp)
	jrno	l6trap
	jrc	l6trap
	jrz	l6trap
	jrnn	l6trap
	ldw	y, (0, sp)
	cpw	y, #0x8000
	jrz	l6
l6trap:
	trap
l6:

	tstw	y
	adcw	0x3ffe
	jro	l7trap
	jrc	l7trap
	jrz	l7trap
	jrnn	l7trap
	ldw	y, (0, sp)
	cpw	y, #0x8001
	jrz	l7
l7trap:
	trap
l7:

	ldw	z, #0x3000
	tstw	y
	adcw	(0xffe, z)
	jro	l8trap
	jrc	l8trap
	jrz	l8trap
	jrnn	l8trap
	ldw	y, (0xffe, z)
	cpw	y, #0x8002
	jrz	l8
l8trap:
	trap
l8:

	ldw	x, #0
	tst	xl	; reset c
	adcw	x
	jro	l9trap
	jrc	l9trap
	jrnz	l9trap
	jrn	l9trap
	cpw	x, #0
	jrz	l9
l9trap:
	trap
l9:
	tstw	x	; set c
	adcw	x
	jro	latrap
	jrc	latrap
	jrz	latrap
	jrn	latrap
	cpw	x, #1
	jrz	la
latrap:
	trap
la:

	ldw	z, #0x00ff
	tstw	z
	adcw	z
	jro	lbtrap
	jrc	lbtrap
	jrz	lbtrap
	jrn	lbtrap
	cpw	z, #0x0100
	jrz	lb
lbtrap:
	trap
lb:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

