.area HOME

	; Test decimal adjust addition.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	push	#0x00
	ld	xl, #0x00
	add	xl, #0x00
	daa	xl
	xch	f, (0, sp)
	cp	xl, #0x00
	jrnz	#l1trap
	ld	xl, (0, sp)
	and	xl, #0x0a	; clear irrelevant flag bits
	cp	xl, #0x08
	jrz	#l1
l1trap:
	trap
l1:

	clr	(0, sp)
	ld	xl, #0x79
	add	xl, #0x11
	daa	xl
	xch	f, (0, sp)
	cp	xl, #0x90
	jrnz	l2trap
	ld	xl, (0, sp)
	and	xl, #0x0a	; clear irrelevant flag bits
	cp	xl, #0x00
	jrz	#l2
l2trap:
	trap
l2:

	clr	(0, sp)
	ld	xh, #0x90
	add	xh, #0x10
	daa	xh
	xch	f, (0, sp)
	cp	xh, #0x00
	jrnz	l3trap
	ld	xl, (0, sp)
	and	xl, #0x0a	; clear irrelevant flag bits
	cp	xl, #0x0a
	jrz	#l3
l3trap:
	trap
l3:

	clr	(0, sp)
	ld	yl, #0x99
	add	yl, #0x01
	daa	yl
	xch	f, (0, sp)
	cp	yl, #0x00
	jrnz	l4trap
	ld	xl, (0, sp)
	and	xl, #0x0a	; clear irrelevant flag bits
	cp	xl, #0x0a
	jrz	#l4
l4trap:
	trap
l4:

	clr	(0, sp)
	ld	zl, #0x09
	add	zl, #0x09
	daa	zl
	xch	f, (0, sp)
	cp	zl, #0x18
	jrnz	l5trap
	ld	xl, (0, sp)
	and	xl, #0x0a	; clear irrelevant flag bits
	cp	xl, #0x00
	jrz	#l5
l5trap:
	trap
l5:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

