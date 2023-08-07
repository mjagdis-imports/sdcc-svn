.area HOME

	; Test 16-bit increment.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0
	incw	y
	jrc	l1trap
	jrn	l1trap
	jro	l1trap
	jrz	l1trap
	cpw	y, #1
	jrz	l1
l1trap:
	trap
l1:

	ldw	x, #0xffff
	incw	x
	jrnc	l2trap
	jrn	l2trap
	jro	l2trap
	jrnz	l2trap
	cpw	x, #0x0000
	jrz	l2
l2trap:
	trap
l2:

	ldw	z, #0x7fff
	incw	z
	jrc	l3trap
	jrnn	l3trap
	jrno	l3trap
	jrz	l3trap
	cpw	z, #0x8000
	jrz	l3
l3trap:
	trap
l3:

	pushw	#0xffff
	incw	(0, sp)
	jrnc	l4trap
	jrn	l4trap
	jro	l4trap
	jrnz	l4trap
	incw	(0, sp)
	jrc	l4trap
	jrn	l4trap
	jro	l4trap
	jrz	l4trap
	ldw	y, (0, sp)
	cpw	y, #0x0001
	jrz	l4
l4trap:
	trap
l4:

	pushw	#0xffff
	incw	0x3ffc
	jrnc	l5trap
	jrn	l5trap
	jro	l5trap
	jrnz	l5trap
	incw	0x3ffc
	jrc	l5trap
	jrn	l5trap
	jro	l5trap
	jrz	l5trap
	ldw	y, 0x3ffc
	cpw	y, #0x0001
	jrz	l5
l5trap:
	trap
l5:

	pushw	#0xffff
	ldw	z, #0x3ffa
	incw	(0, z)
	jrnc	l6trap
	jrn	l6trap
	jro	l6trap
	jrnz	l6trap
	incw	(0, z)
	jrc	l6trap
	jrn	l6trap
	jro	l6trap
	jrz	l6trap
	ldw	y, (0, z)
	cpw	y, #0x0001
	jrz	l6
l6trap:
	trap
l6:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

