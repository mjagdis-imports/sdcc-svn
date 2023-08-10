.area HOME

	; Test 8-bit arithemtic right-shift.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	x, #0x5500
	push	#0x00
	xch	f, (0, sp)
	sra	xl
	xch	f, (0, sp)
	cpw	x, #0x5500
	jrnz	l1trap
	ld	xl, (0, sp)
	and	xl, #0x1f
	cp	xl, #0x08
	jrnz	l1trap
	ld	xl, #0x81
	push	#0x00
	xch	f, (0, sp)
	sra	xl
	xch	f, (0, sp)
	cpw	x, #0x55c0
	jrnz	l1trap
	ld	xl, (0, sp)
	cp	xl, #0x02
	jrz	l1
l1trap:
	trap
l1:

	ldw	x, #0x015a
	push	#0x00
	xch	f, (0, sp)
	sra	xh
	xch	f, (0, sp)
	cpw	x, #0x005a
	jrnz	l2trap
	ld	xl, (0, sp)
	and	xl, #0x1f
	cp	xl, #0x0a
	jrnz	l2trap
	ldw	y, #0xa581
	push	#0x00
	xch	f, (0, sp)
	sra	yl
	xch	f, (0, sp)
	cpw	y, #0xa5c0
	jrnz	l2trap
	ld	xl, (0, sp)
	cp	xl, #0x02
	jrz	l2
l2trap:
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

