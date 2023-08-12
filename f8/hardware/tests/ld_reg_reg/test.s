.area HOME

	; Test 8-bit register-from-register load

	; No prefixes
	ldw	x, #0xa5a6
	ldw	y, #0xa7a8
	ldw	z, #0xa9aa
	ld	xl, xh
	cpw	x, #0xa5a5
	jrnz	l1trap
	ld	xl, yl
	cp	xl, #0xa8
	jrnz	l1trap
	ld	xl, yh
	cp	xl, #0xa7
	jrnz	l1trap
	ld	xl, zl
	cp	xl, #0xaa
	jrnz	l1trap
	ld	xl, zh
	cp	xl, #0xa9
	jrz	l1
l1trap:
	trap
l1:

	; Swap prefix
	ldw	x, #0xa5a6
	ldw	y, #0xa7a8
	ldw	z, #0xa9aa
	ld	xh, xl
	cpw	x, #0xa6a6
	jrnz	l2trap
	ld	yl, xl
	cpw	y, #0xa7a6
	jrnz	l2trap
	ld	yh, xl
	cpw	y, #0xa6a6
	jrnz	l2trap
	ld	zl, xl
	cpw	z, #0xa9a6
	jrnz	l2trap
	ld	zh, xl
	cpw	z, #0xa6a6
	jrz	l2
l2trap:
	trap
l2:	

	; altacc prefix
	ldw	x, #0xa5a6
	ldw	y, #0xa7a8
	ldw	z, #0xa9aa
	ld	xh, yl
	cpw	x, #0xa8a6
	jrnz	l3trap
	ld	xh, yh
	cp	xh, #0xa7
	jrnz	l3trap
	ld	xh, zl
	cp	xh, #0xaa
	jrnz	l3trap
	ld	xh, zh
	cp	xh, #0xa9
	jrz	l3
l3trap:
	trap
l3:

	; altacc' prefix
	ldw	x, #0xa5a6
	ldw	y, #0xa7a8
	ldw	z, #0xa9aa
	ld	yl, xh
	cpw	y, #0xa7a5
	jrnz	l4trap
	ld	yl, yh
	cpw	y, #0xa7a7
	jrnz	l4trap
	ld	yl, zl
	cpw	y, #0xa7aa
	jrnz	l4trap
	ld	yl, zh
	cpw	y, #0xa7a9
	jrz	l4
l4trap:
	trap
l4:

	; altacc'' prefix
	ldw	x, #0xa5a6
	ldw	y, #0xa7a8
	ldw	z, #0xa9aa
	ld	zl, xh
	cpw	z, #0xa9a5
	jrnz	l5trap
	ld	zl, yl
	cp	zl, #0xa8
	jrnz	l5trap
	ld	zl, yh
	cp	zl, #0xa7
	jrnz	l5trap
	ld	zl, zh
	cp	zl, #0xa9
	jrz	l5
l5trap:
	trap
l5:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

