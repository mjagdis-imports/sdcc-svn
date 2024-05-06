.area HOME

	; Test 16-bit loads into accumulator

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	; ldw y, #ii
	ldw	y, #0xa55a
	cpw	y, #0xa55a
	jrnz	l1trap
	ldw	x, #0xa11a
	cpw	x, #0xa11a
	jrnz	l1trap
	cpw	y, #0xa55a
	jrnz	l1trap
	ldw	z, #0xa33a
	cpw	z, #0xa33a
	jrz	l1
l1trap:
	trap
l1:

	; ldw	y, mm
	pushw	#0x5445
	pushw	#0x6446
	pushw	#0x7447
	ldw	y, 0x3ffe
	cpw	y, #0x5445
	jrnz	l2trap
	cpw	x, #0xa11a
	jrnz	l2trap
	ldw	x, 0x3ffc
	cpw	x, #0x6446
	jrnz	l2trap
	ldw	z, 0x3ffa
	cpw	z, #0x7447
	jrz	l2
l2trap:
	trap
l2:

	; ldw	y, (n, sp)
	clrw	y
	clrw	x
	clrw	z
	ldw	y, (0, sp)
	ldw	x, (1, sp)
	ldw	z, (2, sp)
	cpw	y, #0x7447
	jrnz	l3trap
	cpw	x, #0x4674
	jrnz	l3trap
	cpw	z, #0x6446
	jrz	l3
l3trap:
	trap
l3:

	; ldw y, (nn, z)
	ldw	z, #0x300e
	ldw	y, (0x0ff0, z)
	cpw	y, #0x5445
	jrnz	l4trap
	cpw	x, #0x4674
	jrnz	l4trap
	ldw	z, (0x0fee, z)
	cpw	z, #0x6446
	jrz	l4
l4trap:
	trap
l4:

	; ldw	y, (n, y)
	ldw	y, #0x3f00
	ldw	y, (0xfc, y)
	cpw	y, #0x6446
	jrnz	l5trap
	ldw	y, #0x3f00
	ldw	x, (0xfe, y)
	cpw	x, #0x5445
	jrnz	l5trap
	ldw	z, (0xfe, y)
	cpw	z, #0x5445
	jrz	l5
l5trap:
	trap
l5:

	; ldw	y, (y)
	ldw	y, #0x3ffe
	ldw	y, (y)
	cpw	y, #0x5445
	jrnz	l6trap
	ldw	y, #0x3ffc
	ldw	x, (0, y)
	cpw	x, #0x6446
	jrnz	l6trap
	ldw	z, (0, y)
	cpw	z, #0x6446
	jrz	l6
l6trap:
	trap
l6:

	; ldw	y, x
	addw	x, #0x0101
	ldw	y, x
	cpw	y, #0x6547
	jrnz	l7trap
	ldw	y, #0xa55a
	cpw	y, #0xa55a
	jrnz	l7trap
	ldw	x, y
	cpw	x, #0xa55a
	jrnz	l7trap
	ldw	z, y
	cpw	z, #0xa55a
	jrnz	l7trap
	incw	z
	ldw	y, z
	cpw	z, #0xa55b
	jrz	l7
l7trap:
	trap
l7:

	; ldw y, #d
	ldw	y, #-1
	cpw	y, #-1
	jrnz	l8trap
	ldw	y, #0x7f
	cpw	y, #0x7f
	jrnz	l8trap
	ldw	z, #-2
	cpw	z, #-2
	jrnz	l8trap
	ldw	x, #0x7e
	cpw	x, #0x7e
	jrnz	l8trap
	ldw	z, #7
	cpw	z, #7
	jrz	l8
l8trap:
	trap
l8:

	; Load from memory updates n and z, but neither o nor c.
	ld	xl, #0x00
	tst	xl
	sll	xl
	pushw	#0x8000
	ldw	y, (0, sp)
	jrz	l9trap
	jrnn	l9trap
	jrc	l9trap
	jrno	l9n1
l9trap:
	trap
l9n1:
	pushw	#0x0000
	ld	xl, #0x83
	tst	xl
	sll	xl
	ldw	y, (0, sp)
	jrnz	l9trap
	jrn	l9trap
	jrnc	l9trap
	jrno	l9trap
l9:

	; altacc': ldw	z, (z), altacc'': ldw	x, (x)
	clrw	y
	ldw	z, #0x3ffe
	ldw	z, (z)
	cpw	z, #0x5445
	jrnz	latrap
	ldw	x, #0x3ffc
	ldw	x, (x)
	cpw	x, #0x6446
	jrz	la
latrap:
	trap
la:

	ldw	x, #0x0102
	ldw	y, #0x0304
	ldw	z, #0x0506	
	tstw	y
	ldw	z, x
	cpw	z, #0x0102
	jrnz	lbtrap
	incw	z
	ldw	x, z
	cpw	x, #0x0103
	jrnz	lbtrap
	cpw	y, #0x0304
	jrz	lb
lbtrap:
	trap
lb:

	ldw	y, #0xaa55
	pushw	#0xa55a
	clrw	y
	tstw	y	; set z flag
	ldw	y, sp
	ldw	y, (y)
	jrz	lctrap
	cpw	y, #0xa55a
	jrnz	lctrap
	clrw	(0, sp)
	ldw	y, sp
	ldw	y, (y)
	jrnz	lctrap
	tstw	y
	jrz	lc
lctrap:
	trap
lc:

	ldw	y, #0xaa55
	pushw	#0xa55a
	clrw	y
	tstw	y	; set z flag
	ldw	y, sp
	ldw	y, (0, y)
	jrz	ldtrap
	cpw	y, #0xa55a
	jrnz	ldtrap
	clrw	(0, sp)
	ldw	y, sp
	ldw	y, (0, y)
	jrnz	ldtrap
	tstw	y
	jrz	lc
ldtrap:
	trap
ld:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

