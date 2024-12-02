.area HOME

	; Test ldwi

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	x, #0x0102
	tst	xl	; reset c
	pushw	#0x0000
	pushw	#0xa55a
	pushw	#0x5500
	ldw	z, sp
	pushw	#0xaaaa
	pushw	#0xaaaa
	pushw	#0xaaaa
	ldw	y, sp
	ldwi	(0, y), (z)
	incnw	y
	incnw	y
	jrz	l1trap
	jrn	l1trap
	jrc	l1trap
	tstw	x	; set c
	ldwi	(0, y), (z)
	incnw	y
	incnw	y
	jrz	l1trap
	jrnn	l1trap
	jrnc	l1trap
	ldwi	(0, y), (z)
	incnw	y
	incnw	y
	jrnz	l1trap
	jrn	l1trap
	jrnc	l1trap
	cpw	z, #0x4000
	jrnz	l1trap
	cpw	y, #0x4000-6
	jrnz	l1trap
	popw	y
	cpw	y, #0x5500
	jrnz	l1trap
	popw	y
	cpw	y, #0xa55a
	jrnz	l1trap
	popw	y
	cpw	y, #0x0000
	jrnz	l1trap
	cpw	x, #0x0102
	jrz	l1
l1trap:
	trap
l1:

	pushw	#0x0102
	pushw	#0x0000
	ldw	z, sp
	pushw	#0x0304
	ldw	y, sp
	ldwi	(0, y), (z)
	jrnz	l2trap
	jrn	l2trap
	ldwi	(2, y), (z)
	jrz	l2trap
	jrn	l2trap
	popw	y
	cpw	y, #0x0000
	jrnz	l2trap
	popw	y
	cpw	y, #0x0102
	jrnz	l2trap
	popw	y
	cpw	y, #0x0102
	jrz	l2
l2trap:
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

