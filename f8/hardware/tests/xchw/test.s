.area HOME

	; Test xchw.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	; Test sp-relative xchw

	pushw	#0xa55a
	ldw	y, #0xb44b
	push	#0x00
	xch	f, (0, sp)
	xchw	y, (1, sp)
	xch	f, (0, sp)
	pop	xl
	cpw	y, #0xa55a
	jrnz	l1trap
	popw	y
	cpw	y, #0xb44b
	jrnz	l1trap
	cp	xl, #0x00
	jrz	l1
l1trap:
	trap
l1:

	pushw	#0xa55a
	ldw	x, #0xb66b
	push	#0x00
	xch	f, (0, sp)
	xchw	x, (1, sp)
	xch	f, (0, sp)
	pop	yl
	cpw	x, #0xa55a
	jrnz	l2trap
	popw	x
	cpw	x, #0xb66b
	jrnz	l2trap
	cp	yl, #0x00
	jrz	l2
l2trap:
	trap
l2:

	pushw	#0xa55a
	ldw	z, #0xb44c
	push	#0x00
	xch	f, (0, sp)
	xchw	z, (1, sp)
	xch	f, (0, sp)
	pop	xl
	cpw	z, #0xa55a
	jrnz	l3trap
	popw	z
	cpw	z, #0xb44c
	jrnz	l3trap
	cp xl, #0x00
	jrz	l3
l3trap:
	trap
l3:

	; Test other xchw

	pushw	#0xa55a
	ldw	x, #0xb44b
	ldw	y, sp
	push	#0x00
	xch	f, (0, sp)
	xchw	x, (y)
	xch	f, (0, sp)
	pop	yl
	cpw	x, #0xa55a
	jrnz	l4trap
	popw	x
	cpw	x, #0xb44b
	jrnz	l4trap
	cp	yl, #0x00
	jrz	l4
l4trap:
	trap
l4:

	pushw	#0xa56b
	ldw	x, #0xb34b
	ldw	z, sp
	push	#0x00
	xch	f, (0, sp)
	xchw	x, (z)
	xch	f, (0, sp)
	pop	yl
	cpw	x, #0xa56b
	jrnz	l5trap
	popw	x
	cpw	x, #0xb34b
	jrnz	l5trap
	cp	yl, #0x00
	jrz	l5
l5trap:
	trap
l5:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.
