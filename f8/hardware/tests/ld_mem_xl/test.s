.area HOME

	; Test 8-bit memory-from-register loads

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	; no prefix
	pushw	#0x55aa
	ld	xl, #0xa5
	ld	(0, sp), xl
	ld	xl, #0x5a
	ld	(1, sp), xl
	ldw	y, (0, sp)
	cpw	y, #0x5aa5
	jrnz	l1trap
	ld	xl, #0xaa
	ld	0x3ff0, xl	; there used to be a bug in advancing of the pc. With the lower byte of the destiantion being 0xf0, this will trap for the bug.
	ld	0x3fff, xl
	ld	xl, #0x55
	ld	0x3ffe, xl
	ldw	y, (0, sp)
	cpw	y, #0xaa55
	jrnz	l1trap
	ldw	z, #0x3ff0
	ld	xl, #0xa5
	ld	(0xf, z), xl
	ld	xl, #0x5a
	ld	(0xe, z), xl
	ldw	y, (0, sp)
	cpw	y, #0xa55a
	jrnz	l1trap
	ldw	y, #0x3ffe
	ld	xl, #0xaa
	ld	(y), xl
	ld	xl, #0x55
	ld	(1, y), xl
	ldw	y, (y)
	cpw	y, #0x55aa
	jrz	l1
l1trap:
	trap
l1:	

	; altacc
	ld	xh, #0xa5
	ld	(0, sp), xh
	ld	xh, #0x5a
	ld	(1, sp), xh
	ldw	y, (0, sp)
	cpw	y, #0x5aa5
	jrnz	l2trap
	ld	xh, #0xaa
	ld	0x3fff, xh
	ld	xh, #0x55
	ld	0x3ffe, xh
	ldw	y, (0, sp)
	cpw	y, #0xaa55
	jrnz	l2trap
	ldw	z, #0x3ff0
	ld	xh, #0xa5
	ld	(0xf, z), xh
	ld	xh, #0x5a
	ld	(0xe, z), xh
	ldw	y, (0, sp)
	cpw	y, #0xa55a
	jrnz	l2trap
	ldw	y, #0x3ffe
	ld	xh, #0xaa
	ld	(y), xh
	ld	xh, #0x55
	ld	(1, y), xh
	ldw	y, (y)
	cpw	y, #0x55aa
	jrz	l2
l2trap:
	trap
l2:	

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

