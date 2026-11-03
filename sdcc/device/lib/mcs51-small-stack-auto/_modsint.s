	.module _modsint

	.define a0	"dpl"
	.define a1	"dph"

	ar0 = 0			; BUG register set is not considered
	ar1 = 1

	.globl __modsint

	.area	CSEG (CODE)
__modsint:

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

	mov	a,sp
	add	a,#-2		; 2 bytes return address
	mov	r0,a		; r0 points to b1
	mov	a,@r0		; b1

	jnb	acc.7,b_not_negative

	dec	r0

	clr	a
	clr	c
	subb	a,@r0		; b0
	mov	@r0,a
	clr	a
	inc	r0
	subb	a,@r0		; b1
	mov	@r0,a

b_not_negative:

	mov	ar1,@r0		; b1
	dec	r0
	mov	ar0,@r0		; b0

	lcall	__modint

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
