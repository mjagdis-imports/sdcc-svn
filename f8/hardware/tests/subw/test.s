.area HOME

	; Test 16-bit subtraction

	ldw	y, #1
	ldw	x, y
	subw	y, x
	jrnz	#l1trap
	jrn	#l1trap
	cpw	y, #0
	jrz	#l1
l1trap:
	trap
l1:

	ldw	y, #1
	ldw	x, y
	ldw	y, #2
	subw	y, x
	jrz	#l2trap
	jrn	#l2trap
	cpw	y, #1
	jrz	#l2
l2trap:
	trap
l2:

	ldw	y, #1
	ldw	x, y
	ldw	y, #0
	subw	y, x
	jrz	#l3trap
	jrnn	#l3trap
	cpw	y, #-1
	jrz	#l3
l3trap:
	trap
l3:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

