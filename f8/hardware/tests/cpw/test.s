.area HOME

	; Test the cpw instruction.

	ldw	y, #0
	cpw	y, #0
	jrz	#l1
	trap
l1:

	ldw	y, #1
	cpw	y, #1
	jrz #l2
	trap
l2:

loop:
	jr	#loop

