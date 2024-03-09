.area HOME

	; Test the cp instruction.

	ld	xl, #0
	cp	xl, #0
	jrz	#l1
	trap
l1:

	ld	xl, #1
	cp	xl, #1
	jrz #l2
	trap
l2:

	ldw	y, #1
	cp	xl, yl
	jrz #l3
	trap
l3:

	ld	xl, #0
	cp	xl, #1
	jrnz #l4
	trap
l4:

	cp	xl, yl
	jrnz	#l5
	trap
l5:

	cp	xl, yh
	jrz	#l6
	trap
l6:

loop:
	jr	#loop

