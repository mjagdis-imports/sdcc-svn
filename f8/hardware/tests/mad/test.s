.area HOME

	; Test multiply-and-add.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	x, #0x00ff
	push	#0x01
	ld	yl, #0x01
	tst	xl
	mad	x, 0x3fff, yl
	jrc	l1trap
	jrz	l1trap
	cpw	x,	#0x0001
	jrz	l1
l1trap:
	trap
l1:

	ldw	x, #0x00ff
	ld	yl, #0x01
	tst	xl
	mad	x, (0, sp), yl
	jrc	l2trap
	jrz	l2trap
	cpw	x,	#0x0001
	jrz	l2
l2trap:
	trap
l2:

	ldw	x, #0x00ff
	ldw	z, #0x3ffe
	ld	yl, #0x01
	tst	xl
	mad	x, (1, z), yl
	jrc	l3trap
	jrz	l3trap
	jrn	l3trap
	cpw	x,	#0x0001
	jrz	l3
l3trap:
	trap
l3:

	ldw	x, #0x00ff
	ldw	z, #0x3fff
	ld	yl, #0x01
	tst	xl
	mad	x, (z), yl
	jrc	l4trap
	jrz	l4trap
	jrn	l4trap
	cpw	x,	#0x0001
	jrz	l4
l4trap:
	trap
l4:

	ldw	x, #0x02ff
	push	#0x02
	ld	yl, #0x02
	tst	xl
	mad	x, (0, sp), yl
	jrc	l5trap
	jrz	l5trap
	jrn	l5trap
	cpw	x,	#0x0006
	jrz	l5
l5trap:
	trap
l5:

	ldw	x, #0xffff
	push	#0xff
	ld	yl, #0xff
	tst	xl
	mad	x, (0, sp), yl
	jrc	l6trap
	jrz	l6trap
	jrnn	l6trap
	cpw	x,	#0xff00
	jrz	l5
l6trap:
	trap
l6:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

