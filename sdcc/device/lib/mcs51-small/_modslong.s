;-------------------------------------------------------------------------
;  _modslong.s - routine for modulus of 32 bit signed long
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

	.module _modslong
	.include "config.inc"

; Signed and unsigned multiplication are the same - as long as the output
; has the same precision as the input.

	.globl __modslong

	.define a0	"dpl"
	.define a1	"dph"
	.define a2	"b"
	.define a3	"r1"


.ifdef __SDCC_PARMS_IN_BANK1
	.define b0      "b1_0"
	.define b1      "b1_1"
	.define b2      "b1_2"
	.define b3      "b1_3"
.else
	; _modslong_PARM_2 shares the same memory with _modulong_PARM_2
	; and is defined in _modulong.s

	.define b0      "__modslong_PARM_2"
	.define b1      "__modslong_PARM_2 + 1"
	.define b2      "__modslong_PARM_2 + 2"
	.define b3      "__modslong_PARM_2 + 3"
.endif

	.area CSEG    (CODE)

__modslong:
				; a3 in acc
				; b3 in (__modslong_PARM_2 + 3)
	mov	a3,a		; save a3

	clr	F0 		; Flag 0 in PSW
				; available to user for general purpose
	jnb	acc.7,a_not_negative

	setb	F0

	clr	a		; a = -a;
	clr	c
	subb	a,a0
	mov	a0,a
	clr	a
	subb	a,a1
	mov	a1,a
	clr	a
	subb	a,a2
	mov	a2,a
	clr	a
	subb	a,a3
	mov	a3,a

a_not_negative:

	mov	a,b3
	jnb	acc.7,b_not_negative

	clr	a		; b = -b;
	clr	c
	subb	a,b0
	mov	b0,a
	clr	a
	subb	a,b1
	mov	b1,a
	clr	a
	subb	a,b2
	mov	b2,a
	clr	a
	subb	a,b3
	mov	b3,a

b_not_negative:

	mov	a,a3		; restore a3 in acc

	lcall	__modulong

	jnb	F0,not_negative

				; result in (a == r1), b, dph, dpl
	clr	a
	clr	c
	subb	a,a0
	mov	a0,a
	clr	a
	subb	a,a1
	mov	a1,a
	clr	a
	subb	a,a2
	mov	a2,a
	clr	a
	subb	a,a3
				; result in a, b, dph, dpl
not_negative:
	ret

