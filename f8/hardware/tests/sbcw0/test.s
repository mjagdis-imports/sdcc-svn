.area HOME

	; Test wide one opeand subtract with carry.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	y, #0
	tst	xl	; reset c
	sbcw	y
	jro	l1trap
	jrc	l1trap
	jrz	l1trap
	jrnn	l1trap
	cpw	y, #0xffff
	jrz	l1
l1trap:
	trap
l1:

	ldw	y, #0
	tstw	y	; set c
	sbcw	y
	jro	l2trap
	jrnc	l2trap
	jrnz	l2trap
	jrn	l2trap
	cpw	y, #0x0000
	jrz	l2
l2trap:
	trap
l2:

	ldw	y, #0x8000
	tst	xl	; reset c
	sbcw	y
	jrno	l3trap
	jrnc	l3trap
	jrz	l3trap
	jrn	l3trap
	cpw	y, #0x7fff
	jrz	l3
l3trap:
	trap
l3:

	ldw	x, #0
	tst	xl	; reset c
	sbcw	x
	jro	l4trap
	jrc	l4trap
	jrz	l4trap
	jrnn	l4trap
	cpw	x, #0xffff
	jrz	l4
l4trap:
	trap
l4:

	ldw	x, #0
	tstw	y	; set c
	sbcw	x
	jro	l5trap
	jrnc	l5trap
	jrnz	l5trap
	jrn	l5trap
	cpw	x, #0x0000
	jrz	l5
l5trap:
	trap
l5:

	ldw	x, #0x8000
	tst	xl	; reset c
	sbcw	x
	jrno	l6trap
	jrnc	l6trap
	jrz	l6trap
	jrn	l6trap
	cpw	x, #0x7fff
	jrz	l6
l6trap:
	trap
l6:

	ldw	y, #0x55aa
	pushw	#0x0000
	tst	xl	; reset c
	sbcw	(0, sp)
	jro	l7trap
	jrc	l7trap
	jrz	l7trap
	jrnn	l7trap
	cpw	y, #0x55aa
	jrz	l7
l7trap:
	trap
l7:
	popw	y
	cpw	y, #0xffff
	jrnz	l7trap

	ldw	y, #0x55aa
	pushw	#0x0000
	tstw	y	; set c
	sbcw	(0, sp)
	jro	l8trap
	jrnc	l8trap
	jrnz	l8trap
	jrn	l8trap
	cpw	y, #0x55aa
	jrz	l8
l8trap:
	trap
l8:
	popw	y
	cpw	y, #0x0000
	jrnz	l8trap

	ldw	y, #0x55aa
	pushw	#0x8000
	tst	xl	; reset c
	sbcw	(0, sp)
	jrno	l9trap
	jrnc	l9trap
	jrz	l9trap
	jrn	l9trap
	cpw	y, #0x55aa
	jrz	l9
l9trap:
	trap
l9:
	popw	y
	cpw	y, #0x7fff
	jrnz	l9trap

	ldw	y, #0x55aa
	pushw	#0x0000
	tst	xl	; reset c
	sbcw	0x3ffe
	jro	latrap
	jrc	latrap
	jrz	latrap
	jrnn	latrap
	cpw	y, #0x55aa
	jrz	la
latrap:
	trap
la:
	popw	y
	cpw	y, #0xffff
	jrnz	latrap

	ldw	y, #0x55aa
	pushw	#0x0000
	tstw	y	; set c
	sbcw	0x3ffe
	jro	lbtrap
	jrnc	lbtrap
	jrnz	lbtrap
	jrn	lbtrap
	cpw	y, #0x55aa
	jrz	lb
lbtrap:
	trap
lb:
	popw	y
	cpw	y, #0x0000
	jrnz	lbtrap

	ldw	y, #0x55aa
	pushw	#0x8000
	tst	xl	; reset c
	sbcw	0x3ffe
	jrno	lctrap
	jrnc	lctrap
	jrz	lctrap
	jrn	lctrap
	cpw	y, #0x55aa
	jrz	lc
lctrap:
	trap
lc:
	popw	y
	cpw	y, #0x7fff
	jrnz	lctrap

	ldw	z, #0x00fe

	ldw	y, #0x55aa
	pushw	#0x0000
	tst	xl	; reset c
	sbcw	(0x3f00, z)
	jro	ldtrap
	jrc	ldtrap
	jrz	ldtrap
	jrnn	ldtrap
	cpw	y, #0x55aa
	jrz	ld
ldtrap:
	trap
ld:
	popw	y
	cpw	y, #0xffff
	jrnz	ldtrap

	ldw	y, #0x55aa
	pushw	#0x0000
	tstw	y	; set c
	sbcw	(0x3f00, z)
	jro	letrap
	jrnc	letrap
	jrnz	letrap
	jrn	letrap
	cpw	y, #0x55aa
	jrz	le
letrap:
	trap
le:
	popw	y
	cpw	y, #0x0000
	jrnz	letrap

	ldw	y, #0x55aa
	pushw	#0x8000
	tst	xl	; reset c
	sbcw	(0x3f00, z)
	jrno	lftrap
	jrnc	lftrap
	jrz	lftrap
	jrn	lftrap
	cpw	y, #0x55aa
	jrz	lf
lftrap:
	trap
lf:
	popw	y
	cpw	y, #0x7fff
	jrnz	lftrap
	
loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

