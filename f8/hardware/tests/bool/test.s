.area HOME

	; Test 8-bit cast to bool.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ld	xl, #0x00
	bool	xl
	jrnz	#l1trap
	cp	xl, #0
	jrnz	#l1trap
	ld	xh, #1
l1loop:
	ld	xl, xh
	bool	xl
	jrz	l1trap
	cp	xl, #1
	jrnz	#l1trap
	dec	xh
	jrnz	#loop
	jr	#l1
l1trap:
	trap
l1:

	push	#0x1f
	ld	yl, #0
	xch	f, (0, sp)
	bool	yl
	jrnz	#l2trap
	jrnc	#l2trap
	jrnn	#l2trap
	jrno	#l2trap
	cp	yl, #0
	jrnz	#l2trap
	ld	yl, #2
	clr	(0, sp)
	xch	f, (0, sp)
	bool	yl
	jrz	l2trap
	jrc	l2trap
	jrn	l2trap
	jro	l2trap
	cp	yl, #1
	jrz	#l2
l2trap:
	trap
	l2:

	clr	(0, sp)
	clr	zl
	xch	f, (0, sp)
	bool	zl
	jrnz	#l3trap
	jrc	#l3trap
	jrn	#l3trap
	jro	#l3trap
	cp	zl, #0
	jrnz	#l3trap
	ld	zl, #2
	ld	xl, #0x1f
	ld	(0, sp), xl
	xch	f, (0, sp)
	bool	zl
	jrz	#l3trap
	jrnc	#l3trap
	jrnn	#l3trap
	jrno	#l3trap
	cp	zl, #1
	jrz	#l3
l3trap:
	trap
	l3:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

