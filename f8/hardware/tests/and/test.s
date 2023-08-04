.area HOME

	; Test 8-bit bitwise and.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ld	xl, #0xf5
	and	xl, #0x5f
	jrz #l1trap
	jrn	#l1trap
	cp	xl, #0x55
	jrz	#l1
l1trap:
	trap
l1:

	ld	xl, #0xff
	and	xl, #0x80
	jrz #l2trap
	jrnn	#l2trap
	cp	xl, #0x80
	jrz	#l2
l2trap:
	trap
l2:

	ld	xl, #0x55
	and	xl, #0xaa
	jrnz	#l3trap
	jrn	#l3trap
	cp	xl, #0x00
	jrz	#l3
l3trap:
	trap
l3:

	push	#0xff
	ld	xl, #0x5a
	and	xl, (0, sp)
	jrz	#l4trap
	jrn	#l4trap
	cp	xl, #0x5a
	jrz	#l4
l4trap:
	trap
l4:

	ld	xl, #0x5a
	and	xl, 0x3fff
	jrz	#l5trap
	jrn	#l5trap
	cp	xl, #0x5a
	jrz	#l5
l5trap:
	trap
l5:

	ld	xl, #0x5a
	ldw	z, #0x3f00
	and	xl, (0xff, z)
	jrz	#l6trap
	jrn	#l6trap
	cp	xl, #0x5a
	jrz	#l6
l6trap:
	trap
l6:

	ldw	y, #0xffa5
	and	yl, yh
	jrz	#l7trap
	jrnn	#l7trap
	cpw	y, #0xffa5
	jrz	#l7
l7trap:
	trap
l7:

	ld	xl, #0xf0
	ld	xh, #0x0f
	and	xl, xh
	jrnz	#l8trap
	jrn	#l8trap
	cpw	x, #0x0f00
	jrz	#l8
l8trap:
	trap
l8:	

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

