.area HOME

	; Test 8x8->16 multiplication.

	ldw	y, #0x0101
	mul	y
	jrz	l1trap
	jrn	l1trap
	cpw	y, #0x0001
	jrz	l1
l1trap:
	trap
l1:

	ldw	y, #0xffff
	mul	y
	jrz	l2trap
	jrnn	l2trap
	cpw	y, #0xfe01
	jrz	l2
l2trap:
	trap
l2:

	ldw	y, #0x00ff
	mul	y
	jrnz	l3trap
	jrn	l3trap
	cpw	y, #0x0000
	jrz	l3
l3trap:
	trap
l3:

	ldw	x, #0x0101
	mul	x
	jrz	l4trap
	jrn	l4trap
	cpw	x, #0x0001
	jrz	l4
l4trap:
	trap
l4:

	ldw	z, #0xffff
	mul	z
	jrz	l5trap
	jrnn	l5trap
	cpw	z, #0xfe01
	jrz	l5
l5trap:
	trap
l5:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

