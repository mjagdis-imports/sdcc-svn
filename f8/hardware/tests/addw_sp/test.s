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

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

