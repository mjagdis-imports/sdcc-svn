.area HOME

	; Test 16-bit cast to bool.
	
	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y
	
	ldw	y, #0
	push	#0x17
	xch	f, (0, sp)
	boolw	y
	xch	f, (0, sp)
	cpw	y, #0
	jrnz	l1trap
	ld	xl, (0, sp)
	and	xl, #0x1f	; Mask out hidden flag bits
	cp	xl, #0x1f
	jrz	l1
l1trap:
	trap
l1:

	ldw	y, #0x0100
	push	#0x1f
	xch	f, (0, sp)
	boolw	y
	xch	f, (0, sp)
	cpw	y, #1
	jrnz	l2trap
	ld	xl, (0, sp)
	and	xl, #0x1f	; Mask out hidden flag bits
	cp	xl, #0x17
	jrz	l2
l2trap:
	trap
l2:

	ldw	y, #0xaa55

	ldw	x, #0
	push	#0x17
	xch	f, (0, sp)
	boolw	x
	xch	f, (0, sp)
	cpw	x, #0
	jrnz	l3trap
	ld	xl, (0, sp)
	and	xl, #0x1f	; Mask out hidden flag bits
	cp	xl, #0x1f
	jrz	l3
l3trap:
	trap
l3:

	ldw	z, #0xff00
	push	#0x1f
	xch	f, (0, sp)
	boolw	z
	xch	f, (0, sp)
	cpw	z, #1
	jrnz	l4trap
	ld	xl, (0, sp)
	and	xl, #0x1f	; Mask out hidden flag bits
	cp	xl, #0x17
	jrz	l4
l4trap:
	trap
l4:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

