.area HOME

	; Test 16-bit bitwise or.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0x55aa
	orw	y, #0x55aa
	jrz	#l1trap
	jrn	#l1trap
	cpw	y, #0x55aa
	jrz	#l1
l1trap:
	trap
l1:
	orw	y, #0xaa55
	jrz	#l2trap
	jrnn	#l2trap
	cpw	y, #0xffff
	jrz	#l2
l2trap:
	trap
l2:

	ldw	y, #0x0000
	ldw	x, y
	orw	y, x
	jrnz	#l3trap
	jrn	#l3trap
	cpw	y, #0x0000
	jrz	#l3
l3trap:
	trap
l3:

	pushw	#0x5a5a
	ldw	y, #0xa5a5
	orw	y, 0x3ffe
	jrz	#l4trap
	jrnn	#l4trap
	cpw	y, #0xffff
	jrz	#l4
l4trap:
	trap
l4:

	ldw	y, #0x0505
	orw	y, (0, sp)
	jrz	#l5trap
	jrn	#l5trap
	cpw	y, #0x5f5f
	jrz	#l5
l5trap:
	trap
l5:

	ldw	x, #0x55aa
	orw	x, #0x55aa
	jrz	#l6trap
	jrn	#l6trap
	cpw	x, #0x55aa
	jrz	#l6
l6trap:
	trap
l6:
	orw	x, #0xaa55
	jrz	#l7trap
	jrnn	#l7trap
	cpw	x, #0xffff
	jrz	#l7
l7trap:
	trap
l7:

	ldw	z, #0x1122
	ldw	y, #0x2244
	orw	z, y
	cpw	z, #0x3366
	jrz	l8
l8trap:
	trap
l8:

	ldw	z, #0x1122
	ldw	x, #0x2244
	orw	x, z
	cpw	x, #0x3366
	jrz	l9
l9trap:
	trap
l9:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

