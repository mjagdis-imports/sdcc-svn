.area HOME

	; Test 16-bit compare-and-exchange.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	push	#0x55
	ld	zl, #0x55
	ld	xl, #0xaa
	ldw	y, sp
	push	#0x00
	xch	f, (0, sp)
	cax	(y), zl, xl
	jrnz	l1trap
	xch	f, (0, sp)
	pop	xl
	cp	xl, #0x08
	jrnz	l1trap
	pop	xl
	cp	xl, #0xaa
	jrz	l1
l1trap:
	trap
l1:

	push	#0x55
	ld	zl, #0x55
	ld	xh, #0xaa
	ldw	y, sp
	push	#0x00
	xch	f, (0, sp)
	cax	(y), zl, xh
	jrnz	l2trap
	xch	f, (0, sp)
	pop	xl
	cp	xl, #0x08
	jrnz	l2trap
	pop	xl
	cp	xl, #0xaa
	jrz	l2
l2trap:
	trap
l2:

	push	#0x55
	ld	zl, #0x5a
	ld	xl, #0xaa
	ldw	y, sp
	cax	(y), zl, xl
	jrz	l3trap
	cp	zl, #0x55
	jrz	l3
l3trap:
	trap
l3:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

