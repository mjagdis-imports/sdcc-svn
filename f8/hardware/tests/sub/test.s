.area HOME

	; Test 8-bit subtraction.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ld	xl, #0
	ld	xh, #0
	sub	xl, xh
	jrnc	l1trap
	jrn	l1trap
	jrnz	l1trap
	cp	xl, #0
	jrz	l1
l1trap:
	trap
l1:

	ld	xl, #1
	ld	xh, #1
	sub	xl, xh
	jrnc	l2trap
	jrn	l2trap
	jrnz	l2trap
	cp	xl, #0
	jrz	l2
l2trap:
	trap
l2:

	sub	xl, xh
	jrc	l3trap
	jrnn	l3trap
	jrz	l3trap
	cp	xl, #0xff
	jrz	l3
l3trap:
	trap
l3:

	ld	yl, #4
	ld	xh, #2
	sub	yl, xh
	jrnc	l4trap
	jrn	l4trap
	jrz	l4trap
	cp	yl, #2
	jrz	l4
l4trap:
	trap
l4:

	ld	xl, #0x80
	push	#0x01
	sub	xl, (0, sp)
	jrnc	l5trap
	jrn	l5trap
	jrz	l5trap
	cp	xl, #0x7f
	jrz	l5
l5trap:
	trap
l5:

	ldw	z, #0x3fff
	sub	xl, (0, sp)
	jrnc	l6trap
	jrn	l6trap
	jrz	l6trap
	cp	xl, #0x7e
	jrz	l6
l6trap:
	trap
l6:

	ld	xl, #0x01
	sub	(0, sp), xl
	jrnc	l7trap
	jrn	l7trap
	jrnz	l7trap
	cp	xl, #0x01
	jrnz	l7trap
	ld	xl, (0, sp)
	cp	xl, #0x00
	jrz	l7
l7trap:
	trap
l7:

	ld	xl, #0x01
	ld	(0, sp), xl
	sub	(0, z), xl
	jrnc	l8trap
	jrn	l8trap
	jrnz	l8trap
	cp	xl, #0x01
	jrnz	l8trap
	ld	xl, (0, sp)
	cp	xl, #0x00
	jrz	l8
l8trap:
	trap
l8:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

