.area HOME

	; A 16-bit push.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	pushw	#0xa55a
	ldw	y, (0, sp)
	cpw	y, #0xa55a
	jrnz	l1trap
	ldw	y, sp
	cpw	y, #0x3ffe
	jrz	l1
l1trap:
	trap
l1:

	ldw	y, #0xb55b
	pushw	y
	ldw	y, (0, sp)
	cpw	y, #0xb55b
	jrnz	l2trap
	ldw	y, sp
	cpw	y, #0x3ffc
	jrz	l2
l2trap:
	trap
l2:

	pushw	(2, sp)
	ldw	y, (0, sp)
	cpw	y, #0xa55a
	jrnz	l3trap
	ldw	y, sp
	cpw	y, #0x3ffa
	jrz	l3
l3trap:
	trap
l3:

	pushw	0x3ffc
	ldw	y, (0, sp)
	cpw	y, #0xb55b
	jrnz	l4trap
	ldw	y, sp
	cpw	y, #0x3ff8
	jrz	l4
l4trap:
	trap
l4:

	ldw	z, #0x3ffa
	pushw	(0, z)
	ldw	y, (0, sp)
	cpw	y, #0xa55a
	jrnz	l5trap
	ldw	y, sp
	cpw	y, #0x3ff6
	jrz	l5
l5trap:
	trap
l5:

	ldw	x, #0xb55b
	pushw	x
	ldw	y, (0, sp)
	cpw	y, #0xb55b
	jrnz	l6trap
	ldw	y, sp
	cpw	y, #0x3ff4
	jrz	l6
l6trap:
	trap
l6:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

