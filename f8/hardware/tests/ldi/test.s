.area HOME

	; Test ldwi

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	ldw	x, #0x0102
	tst	xl	; reset c
	push	#0x00
	push	#0xa5
	push	#0x50
	ldw	y, sp
	push	#0xaa
	push	#0xaa
	push	#0xaa
	ldw	z, sp
	ldi	(z), (y)
	incnw	y
	jrz	l1trap
	jrn	l1trap
	jrc	l1trap
	tstw	x	; set c
	ldi	(z), (y)
	incnw	y
	jrz	l1trap
	jrnn	l1trap
	jrnc	l1trap
	ldi	(z), (y)
	incnw	y
	jrnz	l1trap
	jrn	l1trap
	jrnc	l1trap
	cpw	y, #0x4000
	jrnz	l1trap
	cpw	z, #0x4000-3
	jrnz	l1trap
	pop	xl
	cp	xl, #0x50
	jrnz	l1trap
	pop	xl
	cp	xl, #0xa5
	jrnz	l1trap
	pop	xl
	cp	xl, #0x00
	jrnz	l1trap
	cpw	x, #0x0100
	jrz	l1
l1trap:
	trap
l1:

	push	#0x01
	push	#0x00
	ldw	x, sp
	push	#0x03
	ldw	z, sp
	ldi	(z), (x)
	incnw	x
	jrnz	l2trap
	jrn	l2trap
	ldi	(z), (x)
	jrz	l2trap
	jrn	l2trap
	pop	xl
	cp	xl, #0x00
	jrnz	l2trap
	pop	xl
	cp	xl, #0x01
	jrnz	l2trap
	pop	xl
	cp	xl, #0x01
	jrz	l2
l2trap:
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

