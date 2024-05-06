.area HOME

	; Test 16-bit bitwise or.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0x55aa
	xorw	y, #0x55aa
	jrnz	#l1trap
	jrn	#l1trap
	jrno	#l1n
	trap
l1n:
	cpw	y, #0x0000
	jrz	#l1
l1trap:
	trap
l1:
	xorw	y, #0xaa55
	jrz	#l2trap
	jrnn	#l2trap
	jrno	#l2n
	trap
l2n:
	cpw	y, #0xaa55
	jrz	#l2
l2trap:
	trap
l2:

	ldw	y, #0x0000
	ldw	x, y
	xorw	y, x
	jrnz	#l3trap
	jrn	#l3trap
	jrno	l3n
	trap
l3n:
	cpw	y, #0x0000
	jrz	#l3
l3trap:
	trap
l3:

	pushw	#0x5a5a
	ldw	y, #0xa5a5
	xorw	y, 0x3ffe
	jrz	#l4trap
	jrnn	#l4trap
	jrno	#l4n
	trap
l4n:
	cpw	y, #0xffff
	jrz	#l4
l4trap:
	trap
l4:

	ldw	y, #0x0505
	xorw	y, (0, sp)
	jrz	#l5trap
	jrn	#l5trap
	jrno	#l5n
	trap
l5n:
	cpw	y, #0x5f5f
	jrz	#l5
l5trap:
	trap
l5:

	ldw	x, #0x55aa
	xorw	x, #0x55aa
	jrnz	#l6trap
	jrn	#l6trap
	jrno	#l6n
	trap
l6n:
	cpw	x, #0x0000
	jrz	#l6
l6trap:
	trap
l6:
	xorw	x, #0xaa55
	jrz	#l7trap
	jrnn	#l7trap
	jrno	#l7n
	trap
l7n:
	cpw	x, #0xaa55
	jrz	#l7
l7trap:
	trap
l7:

	ldw	z, #0x1122
	ldw	y, #0x2244
	xorw	z, y
	cpw	z, #0x3366
	jrz	l8
l8trap:
	trap
l8:

	ldw	z, #0x1122
	ldw	x, #0x2244
	xorw	x, z
	cpw	x, #0x3366
	jrz	#l9
l9trap:
	trap
l9:

	ldw	y, #0x0000
	ldw	x, #0x0001
	xorw	y, x
	jrno	latrap
	cpw	y, #0x0001
	jrz	#la	
latrap:
	trap
la:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

