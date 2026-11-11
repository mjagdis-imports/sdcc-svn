;-------------------------------------------------------------------------
;  _modsint.s - routine for signed int (16 bit) modulus
;
;  Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
;
;  This library is free software; you can redistribute it and/or modify it
;  under the terms of the GNU General Public License as published by the
;  Free Software Foundation; either version 2, or (at your option) any
;  later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with this library; see the file COPYING. If not, write to the
;  Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;  MA 02110-1301, USA.
;
;  As a special exception, if you link this library with other files,
;  some of which are compiled with SDCC, to produce an executable,
;  this library does not by itself cause the resulting executable to
;  be covered by the GNU General Public License. This exception does
;  not however invalidate any other reasons why the executable file
;  might be covered by the GNU General Public License.
;--------------------------------------------------------------------------

	.module _modsint
	.include "config.inc"

	.globl __modsint

	.define a0	"dpl"
	.define a1	"dph"

	; _modsint_PARM_2 shares the same memory with _moduint_PARM_2
	; and is defined in _moduint.s
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

