.area HOME

	; Test relative jumps.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	; Test basic jump.
	ld	xl, #2
	ld	yh, xl
	dnjnz	yh, l1
l1trap:
	trap
l1:
	dnjnz	yh, l1trap

	; Test alternative acc.
	ld	xh, #2
	dnjnz	xh, l2
l2trap:
	trap
l2:
	dnjnz	xh, l2trap

	ld	xl, #2
	ld	zh, xl
	dnjnz	zh, l3
l3trap:
	trap
l3:
	dnjnz	zh, l3trap

	; Test flags
	ld	xh, #0x02
	push	#0x00
	xch	f, (0, sp)
	dnjnz	xh, l4
l4trap:
	trap
l4:
	xch	f, (0, sp)
	pop	xl
	tst	xl
	jrnz	l4trap
	push	#0x00
	xch	f, (0, sp)
	dnjnz	xh, l4trap
	xch	f, (0, sp)
	pop	xl
	tst	xl
	jrz	l4trap

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

