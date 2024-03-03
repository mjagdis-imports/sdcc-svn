.area HOME

	; Test 8-bit subtraction.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	; test sbc xl, mm
	ldw	x, #0xaa55
	ldw	y, #0xbb66
	ldw	z, #0xcc77
	push	#0x01
	tstw	y	; set carry flag.
	sbc	xl, 0x3fff
	jro	l1trap
	jrnc	l1trap
	jrz	l1trap
	jrn	l1trap
	cpw	x, #0xaa54
	jrnz	l1trap
	cpw	y, #0xbb66
	jrnz	l1trap
	cpw	z, #0xcc77
	jrnz	l1trap
	ld	xl, (0, sp)
	cp	xl, #0x01
	jrz	l1
l1trap:
	trap
l1:

	; test sbc mm, xl
	ld	xl, #0x02
	tst	xl	; reset carry flag.
	sbc	0x3fff, xl
	jro	l2trap
	jrc	l2trap
	jrz	l2trap
	jrnn	l2trap
	cpw	x, #0xaa02
	jrnz	l2trap
	cpw	y, #0xbb66
	jrnz	l2trap
	cpw	z, #0xcc77
	jrnz	l2trap
	ld	xl, (0, sp)
	cp	xl, #0xfe
	jrz	l2
l2trap:
	trap
l2:

	; test sbc xh, mm
	ldw	x, #0xaa55
	ldw	y, #0xbb66
	ldw	z, #0xcc77
	push	#0x03
	tstw	y	; set carry flag.
	sbc	xh, 0x3ffe
	jro	l3trap
	jrnc	l3trap
	jrz	l3trap
	jrnn	l3trap
	cpw	x, #0xa755
	jrnz	l3trap
	cpw	y, #0xbb66
	jrnz	l3trap
	cpw	z, #0xcc77
	jrnz	l3trap
	ld	xl, (0, sp)
	cp	xl, #0x03
	jrz	l3
l3trap:
	trap
l3:

	tst	xl	; reset carry flag.
	sbc	(1, sp), xl	; 0xfe - 0x03 - 1
	jro	l4trap
	jrnc	l4trap
	jrz	l4trap
	jrnn	l4trap
	cpw	x, #0xa703
	jrnz	l4trap
	cpw	y, #0xbb66
	jrnz	l4trap
	cpw	z, #0xcc77
	jrnz	l4trap
	ld	xl, (1, sp)
	cp	xl, #0xfa
	jrz	l4
l4trap:
	trap
l4:

	tstw	y	; set carry flag
	ldw	z, #0x3ffe
	ld	xl, #0x03
	sbc	(0, z), xl
	jro	l5trap
	jrnc	l5trap
	jrnz	l5trap
	jrn	l5trap
	cpw	x, #0xa703
	jrnz	l4trap
	cpw	y, #0xbb66
	jrnz	l4trap
	cpw	z, #0x3ffe
	jrnz	l4trap
	tst 0x3ffe
	jrz	l5
l5trap:
	trap
l5:

	tstw	y	; set carry flag
	sbc	xl, xh
	cp	xl, #0x5c
	jrnz	l6trap
	tstw	y	; set carry flag
	sbc	xl, yl
	cp	xl, #0xf6
	jrz	l6
l6trap:
	trap
l6:

	ldw	x, #0xa502
	ldw	y, #0x0303
	ldw	z, #0xaabb
	tstw	y	; set carry flag
	sbc	yl, xl
	jro	l7trap
	jrz	l7trap
	jrn	l7trap
	jrnc	l7trap
	sbc	yh, xl	
	jro	l7trap
	jrz	l7trap
	jrn	l7trap
	jrnc	l7trap
	cpw	x, #0xa502
	jrnz	l7trap
	cpw	y, #0x0101
	jrnz	l7trap
	cpw	z, #0xaabb
	jrz	l7
l7trap:
	trap
l7:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

