.area HOME

	; Test 8-bit push.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ld	xl, #0x55
	push	xl
	cp	xl, (0, sp)
	jrz	#l1
	trap
l1:

	pop	xl
	cp	xl, #0x55
	jrz	#l2
	trap
l2:

	push	#0xaa
	ld	xl, #0xaa
	cp	xl, (0, sp)
	jrz	#l3
	trap
l3:

	push	(0, sp)
	cp	xl, (0, sp)
	jrz	#l4
	trap
l4:

	push	0x3fff
	cp	xl, (0, sp)
	jrz	#l5
	trap
l5:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

