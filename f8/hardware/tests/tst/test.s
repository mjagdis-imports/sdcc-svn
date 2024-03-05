.area HOME

	; Test the the test: tst instruction.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	x, #0xaa55
	ldw	y, #0xab56
	ldw	z, #0x0001
	tst	xl
	jrc	l1trap
	jrn	l1trap
	jrz	l1trap
	jro	l1trap
	tst	zh
	jrc	l1trap
	jrn	l1trap
	jrnz	l1trap
	jro	l1trap
	tst	xh
	jrc	l1trap
	jrnn	l1trap
	jrz	l1trap
	jro	l1trap
	tst	yl
	jrc	l1trap
	jrn	l1trap
	jrz	l1trap
	jro	l1trap
	tst	zl
	jrc	l1trap
	jrn	l1trap
	jrz	l1trap
	jrno	l1trap
	cpw	x, #0xaa55
	jrnz	l1trap
	cpw	y, #0xab56
	jrnz	l1trap
	cpw	z, #0x0001
	jrz	l1
l1trap:
	trap
l1:

	pushw	#0xab55
	tst	(0, sp)
	jrc	l2trap
	jrn	l2trap
	jrz	l2trap
	jro	l2trap
	tst	(1, sp)
	jrc	l2trap
	jrnn	l2trap
	jrz	l2trap
	jrno	l2trap
	tst	0x3ffe
	jrc	l2trap
	jrn	l2trap
	jrz	l2trap
	jro	l2trap
	tst	0x3fff
	jrc	l2trap
	jrnn	l2trap
	jrz	l2trap
	jrno	l2trap
	cpw	x, #0xaa55
	jrnz	l2trap
	cpw	y, #0xab56
	jrnz	l2trap
	cpw	z, #0x0001
	jrz	l2
l2trap:
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

