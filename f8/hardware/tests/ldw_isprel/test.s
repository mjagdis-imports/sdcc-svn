.area HOME

	; Test ldw ((n, sp)), y

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	pushw	#0xaa55
	pushw	#0x3ffe
	ldw	y, #0x55aa
	ldw	((0, sp)), y
	ld	xl, #0xaa
	cp	xl, (2, sp)
	jrz	#l1
	trap
l1:
	ld	xl, #0x55
	cp	xl, (3, sp)
	jrz	#l2
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

