.area HOME

	; Test 16-bit arithmetic right shift.
	
	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0x0000
	sraw	y
	jrc	l1trap
	jrnz	l1trap
	cpw	y, #0x0000
	jrz	l1
l1trap:
	trap
l1:

	ldw	y, #0x55aa
	sraw	y
	jrc	l2trap
	jrz	l2trap
	cpw	y, #0x2ad5
	jrz	l2
l2trap:
	trap
l2:

	ldw	y, #0xaa55
	sraw	y
	jrnc	l3trap
	jrz	l3trap
	cpw	y, #0xd52a
	jrz	l3
l3trap:
	trap
l3:

	ldw	x, #0x0001
	sraw	x
	jrnc	l4trap
	jrnz	l4trap
	cpw	x, #0x0000
	jrz	l4
l4trap:
	trap
l4:

	ldw	z, #0x55aa
	sraw	z
	jrc	l2trap
	jrz	l2trap
	cpw	z, #0x2ad5
	jrz	l5
l5trap:
	trap
l5:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

