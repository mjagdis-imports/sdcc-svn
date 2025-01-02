.area HOME

	; Test clr.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y
	ldw	x, #0xffff
	ldw	y, #0xffff
	ldw	z, #0xffff
	clr	xl
	cp	xl, #0
	jrz	l1
	trap
l1:
	cp	yl, #0xff
	jrz	l2
	trap
l2:
	cp	xh, #0xff
	jrz	l3
	trap
l3:
	ld	xl, #0xff
	clr	yl
	cp	yl, #0
	jrz	l4
	trap
l4:
	cp	xl, #0xff
	jrz	l5
	trap
l5:

	pushw	#0xffff
	clr	(1, sp)
	clr	xl
	cp	xl, (1, sp)
	jrz	l6
	trap
l6:	
	ld	xl, #0xff
	cp	xl, (0, sp)
	jrz	l7
	trap
l7:

	clr	0x3ffe
	clr	xl
	cp	xl, (1, sp)
	jrz	l8
	trap
l8:	

	ldw	z, #0xaa55
	clr	zh
	cpw	z, #0x0055
	jrz	l9
	trap
l9:

	ldw	y, sp
	pushw	#0xffff
	clr	(1, y)
	clr	xl
	cp	xl, (1, sp)
	jrz	la
	trap
la:	
	ld	xl, #0xff
	cp	xl, (0, sp)
	jrz	lb
	trap
lb:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

