.area HOME

	; Test 16-bit increment.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0
	push	#0x00
	xch	f, (0, sp)
	incnw	y
	xch	f, (0, sp)
	cpw	y, #1
	jrnz	l1trap
	pop	xl
	cp	xl, #0x00
	jrz	l1
l1trap:
	trap
l1:

	ldw	x, #0xffff
	push	#0x1f
	xch	f, (0, sp)
	incnw	x
	xch	f, (0, sp)
	tstw	x
	jrnz	l2trap
	pop	xl
	cp	xl, #0x1f
	jrz	l2
l2trap:
	trap
l2:

	ldw	z, #0x7fff
	incnw	z
	cpw	z, #0x8000
	jrz	l3
l3trap:
	trap
l3:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

