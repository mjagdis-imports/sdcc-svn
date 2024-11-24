.area HOME

	; Test 8-bit rotate left through carry.
	
	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ld	xl, #0x55
	add	xl, #0	; reset carry
	rlc	xl
	jrz	#l1trap
	jrc	#l1trap
	jrnn	#l1trap
	cp	xl, #0xaa
	jrz	#l1
l1trap:
	trap
l1:

	ld	xl, #0x80
	add	xl, #0	; reset carry
	rlc	xl
	jrnz	#l2trap
	jrnc	#l2trap
	jrn	#l2trap
	cp	xl, #0x00
	jrz	#l2
l2trap:
	trap
l2:

	ldw	z, #0x55
	add	xl, #0	; reset carry
	rlc	zl
	jrz	#l3trap
	jrc	#l3trap
	jrnn	#l3trap
	cp	zl, #0xaa
	jrz	#l3
l3trap:
	trap
l3:

	push	#0x55
	add	xl, #0	; reset carry
	rlc	0x3fff
	jrz	#l4trap
	jrc	#l4trap
	jrnn	l4trap
	ld	xl, #0xaa
	cp	xl, 0x3fff
	jrz	#l4
l4trap:
	trap
l4:

	push	#0x55
	add	xl, #0	; reset carry
	rlc	(0, sp)
	jrz	#l5trap
	jrc	#l5trap
	jrnn	#l5trap
	ld	xl, #0xaa
	cp	xl, 0x3ffe
	jrz	#l5
l5trap:
	trap
l5:	

	ldw	y, #0x55
	add	yl, #0	; reset carry
	rlc	yl
	jrz	#l6trap
	jrc	#l6trap
	jrnn	#l6trap
	cp	yl, #0xaa
	jrz	#l6
l6trap:
	trap
l6:

	ldw	z, #0x0516
	add	yl, #0	; reset carry
	rlc	zh
	jrz	#l7trap
	jrc	#l7trap
	jrn	#l7trap
	cpw	z, #0x0a16
	jrz	l7
l7trap:
	trap
l7:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

