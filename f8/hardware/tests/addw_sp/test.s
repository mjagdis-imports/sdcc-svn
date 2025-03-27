.area HOME

	; Test addw sp, #d

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	addw	sp, #-8
	ldw	y, sp
	cpw	y, #0x3ff8
	jrnz	l1trap
	addw	sp, #+8
	ldw	y, sp
	cpw	y, #0x4000
	jrnz	l1trap
	addw	sp, #-1
	ldw	y, sp
	cpw	y, #0x3fff
	jrnz	l1trap
	addw	sp, #-128
	ldw	y, sp
	cpw	y, #0x3f7f
	addw	sp, #-128
	ldw	y, sp
	cpw	y, #0x3eff
	jrnz	l1trap
	addw	sp, #127
	ldw	y, sp
	cpw	y, #0x3f7e
	jrz	l1
l1trap:
	trap
l1:
	ldw	y, #0x4000
	ldw	sp, y
	push	#0x55
	xch	f, (0, sp)
	addw	sp, #-20
	addw	sp, #+20
	xch	f, (0, sp)
	pop	xl
	cp	xl, #0x55
	jrnz	l2trap
	push	#0xaa
	xch	f, (0, sp)
	addw	sp, #-60
	addw	sp, #+60
	xch	f, (0, sp)
	pop	xl
	cp	xl, #0xaa
	jrz	l2
l2trap:
	trap
l2:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

