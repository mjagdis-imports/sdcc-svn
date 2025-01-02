.area HOME

	; Test logic right shift
	
	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ld	xl, #0xaa
	srl	xl
	jrz	#l1trap
	jrc	#l1trap
	jrn	#l1trap
	cp	xl, #0x55
	jrz	#l1
l1trap:
	trap
l1:

	srl	xl
	jrz	#l2trap
	jrnc	#l2trap
	jrn	#l2trap
	cp	xl, #0x2a
	jrz	#l2
l2trap:
	trap
l2:

	ld	yl, #0xaa
	srl	yl
	jrz	#l3trap
	jrc	#l3trap
	jrn	#l3trap
	cp	yl, #0x55
	jrz	#l3
l3trap:
	trap
l3:

	srl	yl
	jrz	#l4trap
	jrnc	#l4trap
	jrn	#l4trap
	cp	yl, #0x2a
	jrz	#l4
l4trap:
	trap
l4:

	pushw	#0xff01
	ldw	y, sp
	srl	(1, sp)
	jrz	l5trap
	jrnc	l5trap
	jrn	l5trap
	ld	xl, #0x7f
	cp	xl, (1, sp)
	jrnz	l5trap
	srl	(0, y)
	jrnz	l5trap
	jrnc	l5trap
	jrn	l5trap
	tst	(0, sp)
	jrz	l5
l5trap:
	trap
l5:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

