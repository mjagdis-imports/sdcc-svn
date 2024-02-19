.area HOME

	; Test 8-bit decrement.

	ld	xl, #0x00
	dec	xl
	jrc	#l1trap
	jrz	#l1trap
	jro	#l1trap
	jrnn	#l1trap
	cp	xl, #0xff
	jrz	#l1
l1trap:
	trap
l1:

	ldw	y, #0x0000
	dec	yl
	jrc	#l2trap
	jrz	#l2trap
	jro	#l2trap
	jrnn	#l2trap
	cpw	y, #0x00ff
	jrz	#l2
l2trap:
	trap
l2:

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y
	ld	xl, #0x00
	push	xl
	dec xl
	dec	(0, sp)
	jrc	#l3trap
	jrz	#l3trap
	jro	#l3trap
	jrnn	#l3trap
	cp	xl, (0, sp)
	jrz	#l3
l3trap:
	trap
l3:

	ld	xl, #0x01
	dec	xl
	jrnc	#l4trap
	jrnz	#l4trap
	jro	#l4trap
	jrn	#l4trap
	cp	xl, #0x00
	jrz	#l4
l4trap:
	trap
l4:

	clr	(0, sp)
	dec	(0, sp)
	dec	0x3fff
	ld	xl, #0xfe
	cp	xl, (0, sp)
	jrz	#l5
l5trap:
	trap
l5:

	ld	xl, #0x80
	dec	xl
	jrnc	#l6trap
	jrno	#l6trap
	jrn	#l6trap
	cp	xl, #0x7f
	jrz	#l6
l6trap:
	trap
l6:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

