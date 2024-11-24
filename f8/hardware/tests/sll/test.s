.area HOME

	; Test 8-bit logix left shift.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	x, #0x1155
	ldw	y, #0x2233
	ldw	z, #0x4466
	sll	xl
	jrz	#l1trap
	jrc	#l1trap
	jrnn	#l1trap
	cpw	x, #0x11aa
	jrnz	#l1trap
	sll	xl
	jrz	#l1trap
	jrnc	#l1trap
	cpw	x, #0x1154
	jrz	#l1
l1trap:
	trap
l1:

	sll	yl
	jrz	#l2trap
	jrc	#l2trap
	jrn	#l2trap
	cpw	y, #0x2266
	jrnz	#l2trap
	push	#0x80
	sll	(0, sp)
	jrnz	#l2trap
	jrnc	#l2trap
	push	#0xc0
	sll	0x3ffe
	jrz	#l2trap
	jrnc	#l2trap
	ldw	y, (0, sp)
	cpw	y, #0x0080
	jrz	#l2
l2trap:
	trap
l2:

	sll	zh
	cpw	z, #0x8866
	jrnz	#l4trap
	jrn	#l4trap
	sll	zl
	cpw	z, #0x88cc
	jrz	#l4
l4trap:
	trap
l4:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

