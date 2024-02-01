.area HOME

	; Test call.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y
	call	#f1
	call	#f2

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

f1:
	ret
	trap

f2:
	call	#f1
	ret
	trap

