.area HOME

	; Test call.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y
	ld	xl, #0
	call	#l1
	cp	xl, #1
	jrnz	l1trap
	call	#l2
l1trap:
	trap

l1:
	ld	xl, #1
	popw	y
	jp	y
	trap

l2:
	ldw	y, #l1
	ld	xl, #0
	call	y
	cp	xl, #1
	jrnz	l1trap

	ldw	y, #l1trap
	ldw	x, #l1
	call	x

	push	#0xa5
	xch	f, (0, sp)
	call	#l3
l3:
	xch	f, (0, sp)
	pop	xl
	cp	xl, #0xa5
	jrz	#l4
l4trap:
	trap
l4:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

