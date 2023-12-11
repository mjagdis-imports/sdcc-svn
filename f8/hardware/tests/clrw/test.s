.area HOME

	; Test clrw.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	; clrw y
	ldw	x, #0xffff
	ldw	y, #0xffff
	ldw	z, #0xffff
	clrw	y
	cpw	y, #0
	jrz	l1
	trap
l1:
	cpw	x, #0xffff
	jrz	l2
	trap
l2:
	cpw	z, #0xffff
	jrz	l3
	trap
l3:
	ldw	y, #0xffff
	clrw	x
	cpw	x, #0
	jrz	l4
	trap
l4:
	cpw	y, #0xffff
	jrz	l5
	trap
l5:
	cpw	z, #0xffff
	jrz	l6
	trap
l6:

	; clrw mem
	pushw	#0xffff
	clrw	(0, sp)
	tstw	(0, sp)
	jrnz	l7trap
	decw	(0, sp)
	clrw	0x3ffe
	tstw	(0, sp)
	jrnz	l7trap
	decw	(0, sp)
	pushw	#0xffff
	clrw	(1, sp)
	ldw	y, (0, sp)
	cpw	y, #0x00ff
	jrnz	l7trap
	ldw	y, (2, sp)
	cpw	y, #0xff00
	jrnz	l7trap
	decw	(1, sp)
	ldw	z, #0x000e
	clrw	(0x3ff0, z)
	tstw	0x3ffe
	jrz	l7
l7trap:
	trap
l7:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

