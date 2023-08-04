.area HOME

	; Test 16-bit addition

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0
	addw	y, #0
	jrc	#l1trap
	jrnz	#l1trap
	jrn	#l1trap
	cpw	y, #0
	jrz	#l1
l1trap:
	trap
l1:

	addw	y, #1
	jrc	#l2trap
	jrz	#l2trap
	jrn	#l2trap
	cpw	y, #1
	jrz	#l2
l2trap:
	trap
l2:

	addw	y, #0xffff
	jrnc	#l3trap
	jrnz	#l3trap
	jrn	#l3trap
	cpw	y, #0
	jrz	#l3
l3trap:
	trap
l3:

	pushw 	#1
	ldw	y, #1
	addw	y, (0, sp)
	jrc	#l4trap
	jrz	#l4trap
	jrn	#l4trap
	cpw	y, #2
	jrz	#l4
l4trap:
	trap
l4:	

	ldw	y, #0x8000
	addw	y, #0x8000
	jrnc	#l5trap
	jrnz	#l5trap
	jrn	#l5trap
	cpw	y, #0x0000
	jrz	#l5
l5trap:
	trap
l5:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

