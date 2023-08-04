.area HOME

	; Test some basic functionality related to the stack pointer

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y
	ldw	y, #0xffff
	ldw	y, sp
	cpw	y, #0x4000
	jrz	#l1
	trap
l1:

	addw	sp, #-1
	ldw	y, sp
	cpw	y, #0x3fff
	jrz	#l2
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

