	.title	f8  Sequential Test

	.area	Prog(rel,con)

	abyt	=	0x0010		; Absolute 1-Byte Value
	awrd	=	0x5432		; Absolute 2-Byte Value

	rbyt	=	. + 0x0010	; Relocatable 1-Byte Value
	rwrd	=	. + 0x5432	; Relocatable 2-Byte Value

	.page
;	.sbttl	Base STM8 Instructions in Numerical Order (Absolute)

	.f8

a8bit2op:

	adc	xl, awrd        ; 18 32 54
	adc	xl, #abyt       ; 19 10
	adc	xl, (abyt, sp)  ; 1a 10
	adc	xl, (awrd, z)   ; 1b 32 54
	adc	xl, xh          ; 1c
	adc	xl, zl          ; 1d
	adc	xl, yl          ; 76
	adc	xl, yh          ; 1f
	adc	zl, xl          ; b9 1d
	adc	xh, xl          ; b9 1c
	adc	xh, zl          ; ec 1d
	adc	xh, yl          ; ec 76
	adc	xh, yh          ; ec 1f
	adc	yl, zl          ; ee 1d
	adc	yl, yl          ; ee 76
	adc	zl, yh          ; 43 1f
	adc	yh, zl          ; f5 1d
	adc	zh, zl          ; 49 1d
	adc	zh, xh          ; 49 1c
	adc	zh, yl          ; 49 76
	adc	zh, yh          ; 49 1f

	add	xl, awrd        ; 10 32 54
	add	xl, #abyt       ; 11 10
	add	xl, (abyt, sp)  ; 12 10
	add	xl, (awrd, z)   ; 13 32 54
	add	xl, xh          ; 14
	add	xl, zl          ; 16
	add	xl, yl          ; 15
	add	xl, yh          ; 17


	and	xl, awrd        ; 70 32 54
	and	xl, #abyt       ; 79 10
	and	xl, (abyt, sp)  ; 72 10
	and	xl, (awrd, z)   ; 73 32 54
	and	xl, xh          ; 7c
	and	xl, zl          ; 75
	and	xl, yl          ; 1e
	and	xl, yh          ; 77

	cp	xl, awrd        ; 20 32 54
	cp	xl, #abyt       ; 21 10
	cp	xl, (abyt, sp)  ; 22 10
	cp	xl, (awrd, z)   ; 23 32 54
	cp	xl, xh          ; 24
	cp	xl, zl          ; 25
	cp	xl, yl          ; 26
	cp	xl, yh          ; 27

	or	xl, awrd        ; 28 32 54
	or	xl, #abyt       ; 29 10
	or	xl, (abyt, sp)  ; 2a 10
	or	xl, (awrd, z)   ; 2b 32 54
	or	xl, xh          ; 2c
	or	xl, zl          ; 2d
	or	xl, yl          ; 2e
	or	xl, yh          ; 2f

	sbc	xl, awrd        ; 08 32 54
	sbc	xl, (abyt, sp)  ; 0a 10
	sbc	xl, (awrd, z)   ; 0b 32 54
	sbc	xl, xh          ; 0c
	sbc	xl, zl          ; 0d
	sbc	xl, yl          ; 0e
	sbc	xl, yh          ; 0f

	sub	xl, awrd        ; 01 32 54
	sub	xl, (abyt, sp)  ; 02 10
	sub	xl, (awrd, z)   ; 03 32 54
	sub	xl, xh          ; 04
	sub	xl, zl          ; 05
	sub	xl, yl          ; 06
	sub	xl, yh          ; 07

	xor	xl, awrd        ; 78 32 54
	xor	xl, #abyt       ; 71 10
	xor	xl, (abyt, sp)  ; 7a 10
	xor	xl, (awrd, z)   ; 7b 32 54
	xor	xl, xh          ; 74
	xor	xl, zl          ; 7d
	xor	xl, yl          ; 7e
	xor	xl, yh          ; 7f

a16bit2op:

	adcw	y, awrd         ; 3c 32 54
	adcw	y, #awrd        ; 3d 32 54
	adcw	y, (abyt, sp)   ; 3e 10
	adcw	y, x            ; 3f

	addw	y, awrd         ; 38 32 54
	addw	y, #awrd        ; 50 32 54
	addw	y, (abyt, sp)   ; 3a 10
	addw	y, x            ; 3b

	orw	y, awrd         ; e0 32 54
	orw	y, #awrd        ; e1 32 54
	orw	y, (abyt, sp)   ; e2 10
	orw	y, x            ; e3

	sbcw	y, awrd         ; 34 32 54
	sbcw	y, (abyt, sp)   ; 36 10
	sbcw	y, x            ; 37

	subw	y, awrd         ; 30 32 54
	subw	y, (abyt, sp)   ; 32 10
	subw	y, x            ; 33

	xorw	y, awrd         ; a1 32 54
	xorw	y, #awrd        ; fd 32 54
	xorw	y, (abyt, sp)   ; fe 10
	xorw	y, x            ; ff

a8bit1op:

	clr	awrd            ; d9 32 54
	clr	(abyt, sp)      ; d8 10
	clr	xl              ; da
	clr	(abyt, y)       ; db 10

	dec	awrd            ; 55 32 54
	dec	(abyt, sp)      ; 54 10
	dec	xl              ; 56
	dec	(abyt, y)       ; 57 10

	inc	awrd            ; 51 32 54
	inc	(abyt, sp)      ; 39 10
	inc	xl              ; 52
	inc	(abyt, y)       ; 53 10

	push	awrd            ; 61 32 54
	push	(abyt, sp)      ; 60 10
	push	xl              ; 62
	push	(abyt, y)       ; 63 10

	sll	awrd            ; d5 32 54
	sll	(abyt, sp)      ; d4 10
	sll	xl              ; d6
	sll	(abyt, y)       ; d7 10

	srl	awrd            ; d1 32 54
	srl	(abyt, sp)      ; d0 10
	srl	xl              ; d2
	srl	(abyt, y)       ; d3 10

	rlc	awrd            ; 5d 32 54
	rlc	(abyt, sp)      ; 5c 10
	rlc	xl              ; 5e
	rlc	(abyt, y)       ; 5f 10

	rrc	awrd            ; 59 32 54
	rrc	(abyt, sp)      ; 58 10
	rrc	xl              ; 5a
	rrc	(abyt, y)       ; 5b 10

	tst	awrd            ; dd 32 54
	tst	(abyt, sp)      ; dc 10
	tst	xl              ; de
	tst	(abyt, y)       ; df 10

a16bit1op:

	adcw	(abyt, sp)	; a8 10
	adcw	awrd            ; a9 32 54
	adcw	(awrd, z)       ; aa 32 54
	adcw	y               ; ab

	clrw	(abyt, sp)	; f0 10
	clrw	awrd            ; f1 32 54
	clrw	(awrd, z)       ; f2 32 54
	clrw	y               ; f3

	incw	(abyt, sp)	; a4 10
	incw	awrd            ; a5 32 54
	incw	(awrd, z)       ; ae 32 54
	incw	y               ; a7

	pushw	(abyt, sp)	; 80 10
	pushw	awrd            ; 81 32 54
	pushw	(awrd, z)       ; 82 32 54
	pushw	y               ; 83

	sbcw	(abyt, sp)	; ac 10
	sbcw	awrd            ; ad 32 54
	sbcw	(awrd, z)       ; a6 32 54
	sbcw	y               ; af

	tstw	(abyt, sp)	; 90 10
	tstw	awrd            ; 91 32 54
	tstw	(awrd, z)       ; 86 32 54
	tstw	y               ; 87

a8ld:

	ld	xl, #abyt       ; e9 10
	ld	xl, awrd        ; c0 32 54
	ld	xl, (abyt, sp)  ; c2 10
	ld	xl, (awrd, z)   ; c3 32 54
	ld	xl, (y)         ; c5
	ld	xl, (abyt, y)   ; c4 10
	ld	xl, xh          ; c6
	ld	xh, xl          ; b9 c6
	ld	xl, yl          ; c7
	ld	yl, xl          ; b9 c7
	ld	xl, yh          ; c9
	ld	yh, xl          ; b9 c9
	ld	xl, zl          ; c8
	ld	zl, xl          ; b9 c8
	ld	xl, zh          ; ca
	ld	zh, xl          ; b9 ca
	ld	awrd, xl        ; cb
	ld	(abyt, sp), xl  ; cd 10
	ld	(awrd, z), xl   ; cc 32 54
	ld	(abyt, y), xl   ; c1 10
	ld	(y), xl         ; ce
	ld	(y), xh         ; ec ce
	ld	(z), yl         ; ee ce
	ld	(x), zl         ; 43 ce
	ld	(z), yh         ; f5 ce
	ld	(y), zh         ; 49 ce
	ldi	(abyt, y), (z)  ; 8c

a16ld:

	ldw	y, #awrd        ; b1
	ldw	y, awrd         ; b0
	ldw	y, (abyt, sp)   ; b2 10
	ldw	y, (awrd, z)    ; b3 32 54
	ldw	y, (abyt, y)    ; b5 10
	ldw	y, (y)          ; b4
	ldw	y, #abyt        ; b7 10
	ldw	x, (y)          ; 66
	ldw	y, (z)          ; ee 66
	ldw	z, (x)          ; 43 66
	ldw     z, (y)          ; 49 66
	ldw	y, x            ; b6
	ldw     y, z            ; 6e
	ldw	x, y            ; bb
	ldw     z, y            ; 85
	ldw	z, x            ; ee b6
	ldw	x, z            ; ee bb
	ldw	awrd, y         ; ed
	ldw	(abyt, sp), y   ; b8 10
	ldw	(awrd, z), y    ; ba 32 54
	ldw	(y), x          ; 84
	ldw	(abyt, y), x    ; 88 10
	ldw	y, sp           ; 31
	ldw	sp, y           ; b9 31
	ldw	((abyt, sp)), y ; 35 10
	ldwi	(abyt, y), (z)  ; 8d 10
	sex	y, xl           ; 8e
	zex	y, xl           ; 8f

ao8:

	bool	xl              ; cf
	cax	(y), zl, xl     ; eb
	da	xl              ; f7
	mad	x, awrd, yl     ; 9d 32 54
	mad	x, (abyt, sp), yl ; fa 10
	mad	x, (awrd, z), yl ; 9e 32 54
	mad	x, (z), yl      ; 9f
	msk	(y), xl, #abyt  ; 99 10
	pop	xl              ; 6d
	push	#abyt           ; fc 10
	rot	xl, #abyt       ; f4 10
	sra	xl              ; f6
	thrd	xl              ; 68
	xch	yl, yh          ; a3
	xch	xl, (abyt, sp)  ; a0 10
	xch	xl, (y)         ; a2
	xch	f, (abyt, sp)   ; bf 10

ao16:

	addw	sp, #abyt       ; 8a 10
	addw	y, #abyt        ; 8b 10
	boolw	y               ; fb
	caxw	(y), z, x       ; f8
	cpw	y, #awrd        ; bc 32 54
	decw	(abyt, sp)      ; 47 10
	incnw	y               ; 46
	negw	y               ; 9c
	mul	y               ; 98
	popw	y               ; be
	pushw	#awrd           ; 89 32 54
	rlcw	y               ; ef
	rlcw	(abyt, sp)      ; e7 10
	rrcw	y               ; 42
	rrcw	(abyt, sp)      ; e6 10
	sllw	y               ; 40
	sllw	y, xl           ; 44
	sraw	y               ; 45
	srlw	y               ; 69
	xchw	x, (y)          ; e5

aj:

	call	#awrd           ; 4e
	call	y               ; 4f
1$:	dnjnz	yh, 1$          ; ea 00
	jp	#awrd           ; 4d 00
	jp	y               ; 4c 00
2$:	jr	2$              ; bd 00
3$:	jrc     3$              ; 65 00
4$:	jrgt    4$              ; b9 41 00
5$:	jrle    5$              ; 41 00
6$:	jrn     6$              ; 96 00
7$:	jrnc    7$              ; 64 00
8$:	jrnn    8$              ; 97 00
9$:	jrno    9$              ; 48 00
a$:	jrnz    a$              ; 93 00
b$:	jro     b$              ; b9 48 00
c$:	jrsge   c$              ; 94 00
d$:	jrsgt   d$              ; b9 4a 00
e$:	jrsle   e$              ; 4a 00
f$:	jrslt   f$              ; 92 00
g$:	jrz     g$              ; 95 00
	ret                     ; 9a
	reti                    ; 9b
	trap                    ; 00

