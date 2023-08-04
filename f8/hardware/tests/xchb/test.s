.area HOME

	; Test ldw ((n, sp)), y

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	pushw	#0x00ff
	clr	xl
	xchb	xl, 0x3ffe, #7
	jrnz #l1
	trap
l1:
	cp	xl, #1
	jrz	#l2
	trap
l2:
	xchb	xl, 0x3ffe, #0
	jrnz #l3
	trap
l3:
	cp	xl, #1
	jrz	#l4
	trap
l4:
	ld	xl, #0x7f
	cp	xl, (0, sp)
	jrz	#l5
	trap
l5:

	ld	xl, #0xff
	xchb	xl, 0x3fff, #7
	jrz #l6
	trap
l6:
	cp	xl, #0
	jrz	#l7
	trap
l7:
	xchb	xl, 0x3fff, #0
	jrz #l8
	trap
l8:
	cp	xl, #0
	jrz	#l9
	trap
l9:
	ld	xl, #0x80
	cp	xl, (1, sp)
	jrz	#la
	trap
la:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

