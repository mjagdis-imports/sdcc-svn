	.module _modsint

	.globl __modsint

	.define a0	"dpl"
	.define a1	"dph"

	; _modsint_PARM_2 shares the same memory with _moduint_PARM_2
	; and is defined in _moduint.c
	.define b0      "(__modsint_PARM_2)"
	.define b1      "(__modsint_PARM_2 + 1)"

	.area	CSEG (CODE)
__modsint:
				; a1 in dph
				; b1 in (__modsint_PARM_2 + 1)

	clr	F0 		; Flag 0 in PSW
				; available to user for general purpose
	mov	a,a1
	jnb	acc.7,a_not_negative

	setb	F0

	clr	a
	clr	c
	subb	a,a0
	mov	a0,a
	clr	a
	subb	a,a1
	mov	a1,a

a_not_negative:

	mov	a,b1
	jnb	acc.7,b_not_negative

	clr	a
	clr	c
	subb	a,b0
	mov	b0,a
	clr	a
	subb	a,b1
	mov	b1,a

b_not_negative:

	lcall	__moduint

	jnb	F0,not_negative

	clr	a
	clr	c
	subb	a,a0
	mov	a0,a
	clr	a
	subb	a,a1
	mov	a1,a

not_negative:
	ret
