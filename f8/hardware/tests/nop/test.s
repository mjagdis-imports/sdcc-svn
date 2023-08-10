.area HOME

	; Test no-op.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	x, #0xaa55
	ldw	y, #0xa55a
	ldw	z, #0x33bb
	push	#0x00
	xch	f, (0, sp)
	nop
	xch	f, (0, sp)
	cpw	x, #0xaa55
	jrnz	#l1trap
	cpw	y, #0xa55a
	jrnz	#l1trap
	cpw	z, #0x33bb
	jrnz	#l1trap
	ld	xl, (0, sp)
	and	xl, #0x1f
	cp	xl, #0x00
	jrnz	#l1trap
	push	#0x1f
	xch	f, (0, sp)
	nop
	xch	f, (0, sp)
	ld	xl, (0, sp)
	and	xl, #0x1f
	cp	xl, #0x1f
	jrz	#l1
l1trap:
	trap
l1:
	
loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

