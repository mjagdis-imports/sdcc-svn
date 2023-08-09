.area HOME

	; Test 8-bit addition with carry.
	
	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y
	
	push	#0x00

	ld	xl, #0
	tst	xl	; reset carry
	adc	xl, #0
	xch	f, (0, sp)
	cp	xl, #0
	jrnz	l1trap
	ld	xl, (0, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x08	; Check: h reset, c reset, n reset, z set, o reset.
	jrz	l1
l1trap:
	trap
l1:

	clr	(0, sp)
	ld	xh, #1
	tst	xl	; reset carry
	adc	xh, #0
	xch	f, (0, sp)
	cp	xh, #1
	jrnz	l2trap
	ld	xl, (0, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x00	; Check: h reset, c reset, n reset, z reset, o reset.
	jrz	l2
l2trap:
	trap
l2:

	clr	(0, sp)
	ld	yl, #0x7f
	tstw	y	; set carry
	adc	yl, #0
	xch	f, (0, sp)
	cp	yl, #0x80
	jrnz	l3trap
	ld	xl, (0, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x15	; Check: h reset, c reset, n set, z reset, o set.
	jrz	l3
l3trap:
	trap
l3:

	clr	(0, sp)
	ld	zl, #0xff
	tstw	y	; set carry
	adc	zl, #0
	xch	f, (0, sp)
	cp	zl, #0x00
	jrnz	l4trap
	ld	xl, (0, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x0b	; Check: h reset, c set, n reset, z set, o reset.
	jrz	l4
l4trap:
	trap
l4:

	push	#0x0f
	clr	(1, sp)
	ld	xl, #0x00
	tstw	y	; set carry
	adc	xl, (0, sp)
	xch	f, (1, sp)
	cp	xl, #0x10
	jrnz	l5trap
	ld	xl, (1, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x01	; Check: h set, c reset, n reset, z reset, o reset.
	jrz	l5
l5trap:
	trap
l5:

	clr	(1, sp)
	ld	xl, #0xf1
	tst	xl	; reset carry
	adc	(0, sp), xl
	xch	f, (1, sp)
	ld	xl, (0, sp)
	cp	xl, #0x00
	jrnz	l6trap
	ld	xl, (1, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x0b	; Check: h set, c set, n reset, z set, o reset.
	jrz	l6
l6trap:
	trap
l6:

	clr	(1, sp)
	ld	xl, #0x70
	ld	zl, #0x10
	tst	xl
	adc	xl, zl
	xch	f, (1, sp)
	cp	xl, #0x80
	jrnz	l7trap
	ld	xl, (1, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x14	; Check: h reset, c reset, n set, z reset, o set.
	jrz	l7
l7trap:
	trap
l7:

	ld	xl, #0x01
	ld	xh, #0x01
	tst	xl
	adc	xh, xl
	cpw	x, #0x0201
	jrz	l8
	trap
l8:

	clr	(1, sp)
	ldw	x, #0x5504
	ldw	y, #0xaa05
	tst	xl
	adc	yl, xl
	cpw	x, #0x5504
	jrnz	l9trap
	cpw	y, #0xaa09
	jrnz	l9trap
	ld	xl, (1, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x00	; Check: h reset, c reset, n reset, z reset, o reset.
	jrz	l9
l9trap:
	trap
l9:

	clr	(1, sp)
	ld	xh, #0x0f
	ld	(0, sp), xh	; sp should point to 0x3ffe here.
	ld	xh, #0x01
	tstw	y	; set carry
	adc	xh, 0x3ffe
	xch	f, (1, sp)
	cp	xh, #0x11
	jrnz	latrap
	ld	xl, (1, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x01	; Check: h set, c reset, n reset, z reset, o reset.
	jrz	#la
latrap:
	trap
la:

	clr	(1, sp)
	ld	yl, #0x0f
	ldw	z, #0x3ffe
	ld	(0, sp), yl	; sp should point to 0x3ffe here.
	ld	yl, #0x01
	tstw	y	; set carry
	adc	yl, (0, z)
	xch	f, (1, sp)
	cp	yl, #0x11
	jrnz	lbtrap
	ld	xl, (1, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x01	; Check: h set, c reset, n reset, z reset, o reset.
	jrz	#lb
lbtrap:
	trap
lb:

	clr	(1, sp)
	ld	xl, #0x88
	ld	(0, sp), xl	; sp should point to 0x3ffe here.
	ld	xl, #0x88
	tst	xl	; reset carry
	adc	0x3ffe, xl
	xch	f, (1, sp)
	ld	xl,	0x3ffe
	cp	xl, #0x10
	jrnz	lctrap
	ld	xl, (1, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x13	; Check: h set, c set, n reset, z reset, o set.
	jrz	#lc
lctrap:
	trap
lc:

	clr	(1, sp)
	ld	xl, #0xff
	ld	(0, sp), xl	; sp should point to 0x3ffe here.
	ld	xl, #0x01
	tst	xl	; reset carry
	adc	(0, z), xl
	xch	f, (1, sp)
	ld	xl,	0x3ffe
	cp	xl, #0x00
	jrnz	ldtrap
	ld	xl, (1, sp)
	and	xl, #0x1f	; clear hidden flags
	cp	xl, #0x0b	; Check: h set, c set, n reset, z set, o reset.
	jrz	#ld
ldtrap:
	trap
ld:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

