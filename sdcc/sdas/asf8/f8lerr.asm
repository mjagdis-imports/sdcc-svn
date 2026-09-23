	.title	f8l Assembly Errors Test

	.area	Prog(rel,con)

	abyt	=	0x0010		; Absolute 1-Byte Value
	awrd	=	0x5432		; Absolute 2-Byte Value

	; f8 instruction not available on f8l
	.f8l

	; y-relative addressing mode
	clr	(abyt, y)
	dec	(abyt, y)
	inc	(abyt, y)
	push	(abyt, y)
	rlc	(abyt, y)
	rrc	(abyt, y)
	sll	(abyt, y)
	srl	(abyt, y)
	tst	(abyt, y)
	ld	xl, (abyt, y)
	ld	(abyt, y), xl
	ldi	(abyt, y), (z)
	ldw	y, (abyt, y)
	ldw	(abyt, y), x
	ldwi	(abyt, y), (z)

	; 16-bit 2-operand instruction
	adcw	y, #awrd
	adcw	y, awrd
	adcw	y, (abyt, sp)
	adcw	y, x
	addw	y, awrd
	addw	y, #awrd
	addw	y, (abyt, sp)
	addw	y, x
	orw	y, #awrd
	orw	y, awrd
	orw	y, (abyt, sp)
	orw	y, x
	sbcw	y, awrd
	sbcw	y, (abyt, sp)
	sbcw	y, x
	subw	y, awrd
	subw	y, (abyt, sp)
	subw	y, x
	xorw	y, #awrd
	xorw	y, awrd
	xorw	y, (abyt, sp)
	xorw	y, x
	cpw	y, #awrd

	; 16-bit 1-operand instruction
	adcw	awrd
	adcw	(abyt, sp)
	adcw	(awrd, z)
	adcw	y
	sbcw	awrd
	sbcw	(abyt, sp)
	sbcw	(awrd, z)
	sbcw	y
	srlw	y
	sllw	y
	rrcw	y
	rlcw	y
	rrcw	(abyt, sp)
	rlcw	(abyt, sp)
	incnw	y
	decw	(abyt, sp)
	negw	y
	boolw	y

	; various instructions
	ldw	((abyt, sp)), y
	xch	xl, (abyt, sp)
	xch	yl, yh
	rot	xl, #2
	mul	y
	mad	x, awrd, yl
	mad	x, (abyt, sp), yl
	mad	x, (awrd, z), yl
	mad	x, (z), yl
1$:	dnjnz	yh, 1$
	sllw	y, xl
	sex	y, xl
	zex	y, xl
	xchw	y, (abyt, sp)

	; instructions where the base version is available on f8l, but not the swap-prefixed one
	cp	#abyt, xl
	cp	awrd, xl
	cp	(abyt, sp), xl
	cp	(awrd, z), xl
	cp	zl, xl
	cp	xh, xl
	cp	yl, xl
	cp	yh, xl
	sbc	awrd, xl
	sbc	(abyt, sp), xl
	sbc	(awrd, z), xl
	sbc	zl, xl
	sbc	xh, xl
	sbc	yl, xl
	sbc	yh, xl
	sub	awrd, xl
	sub	(abyt, sp), xl
	sub	(awrd, z), xl
	sub	zl, xl
	sub	xh, xl
	sub	yl, xl
	sub	yh, xl

