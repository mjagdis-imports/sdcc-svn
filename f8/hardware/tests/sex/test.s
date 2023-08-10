.area HOME

	; Test 8-to-16 bit sign extension

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

