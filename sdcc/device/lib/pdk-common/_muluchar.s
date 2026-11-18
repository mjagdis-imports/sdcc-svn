	.module __muluchar
	.include "config.inc"

	.area DATA

	.area	OSEG (OVR,DATA)
___muluchar_PARM_1::
	.ds 1
___muluchar_PARM_2::
	.ds 1

	.area CODE

; unsigned int __muluchar (unsigned char x, unsigned char y)
___muluchar::

	mov	a, ___muluchar_PARM_1
	mov	p, a
	mov	a, #0x08
	mov	___muluchar_PARM_1, a
	mov	a, #0x00

0$:
	sl	a
	slc	p
	t0sn.io f, c
	add	a, ___muluchar_PARM_2
3$:
	addc	p

	dzsn	___muluchar_PARM_1
	goto	0$
4$:

	ret

