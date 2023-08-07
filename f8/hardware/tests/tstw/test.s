.area HOME

	; Tesst tstw.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0x0000
	tstw	y
	jrnc	l1trap
	jrn	l1trap
	jrnz	l1trap
	jrno	l1
l1trap:
	trap
l1:

	ldw	y, #0x8000
	tstw	y
	jrnc	l2trap
	jrnn	l2trap
	jrz	l2trap
	jro	l2
l2trap:
	trap
l2:

	ldw	x, #0x0000
	tstw	x
	jrnc	l1trap
	jrn	l3trap
	jrnz	l3trap
	jrno	l3
l3trap:
	trap
l3:

	ldw	z, #0x8000
	tstw	z
	jrnc	l4trap
	jrnn	l4trap
	jrz	l4trap
	jro	l4
l4trap:
	trap
l4:

	pushw	#0x0000
	pushw	#0x0001
	pushw	#0x8000
	ldw	z, #0xffa
	tstw	(4, sp)
	jrnc	l5trap
	jrn	l5trap
	jrnz	l5trap
	tstw	0xffe
	jrnc	l5trap
	jrn	l5trap
	jrnz	l5trap
	tstw	(4, z)
	jrnc	l5trap
	jrn	l5trap
	jrnz	l5trap
	tstw	(2, sp)
	jrnc	l5trap
	jrn	l5trap
	jrz	l5trap
	tstw	0xffc
	jrnc	l5trap
	jrn	l5trap
	jrz	l5trap
	tstw	(2, z)
	jrnc	l5trap
	jrn	l5trap
	jrz	l5trap
	tstw	(0, sp)
	jrnc	l5trap
	jrnn	l5trap
	jrz	l5trap
	tstw	0xffa
	jrnc	l5trap
	jrnn	l5trap
	jrz	l5trap
	tstw	(0, z)
	jrnc	l5trap
	jrnn	l5trap
	jrz	l5trap
	jr	l5
l5trap:
	trap
l5:


loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

