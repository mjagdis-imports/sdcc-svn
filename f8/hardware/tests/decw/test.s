.area HOME

	; Test 16-bit decrement

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	pushw	#1
	decw	(0, sp)
	jrnc	#l1trap
	jrnz	#l1trap
	jro	l1trap
	decw	(0, sp)
	jrc	#l1trap
	jrz	#l1trap
	jro	#l1trap
	ldw	y, (0, sp)
	cpw	y, #-1
	jrz	#l1
l1trap:
	trap
l1:

	pushw	#0x8000
	decw	(0, sp)
	jrnc	#l2trap
	jrz	#l2trap
	jrno	#l2trap
	ldw	y, (0, sp)
	cpw	y, #0x7fff
	jrz	#l2
l2trap:
	trap
l2:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

