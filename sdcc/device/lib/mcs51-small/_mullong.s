;-------------------------------------------------------------------------
;  _mullong.s - routine for multiplication of 32 bit (unsigned) long
;
;  Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
;  Copyright (C) 1999, Jean Louis VERN jlvern@writeme.com
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

	.module _mullong
	.include "config.inc"

; Signed and unsigned multiplication are the same - as long as the output
; has the same precision as the input.

	.globl	__mullong
	.globl __mullong_PARM_2


				; the result c will be stored in r4...r7
	.define c0 "r4"
	.define c1 "r5"
	.define c2 "r6"
	.define c3 "r7"

	.define a0 "dpl"
	.define a1 "dph"
	.define a2 "r2"
	.define a3 "r3"

	; c0  a0 * b0
	; c1  a1 * b0 + a0 * b1
	; c2  a2 * b0 + a1 * b1 + a0 * b2
	; c3  a3 * b0 + a2 * b1 + a1 * b2 + a0 * b3

.ifdef __SDCC_PARMS_IN_BANK1
	.define b0  "b1_0"
	.define b1  "b1_1"
	.define b2  "b1_2"
	.define b3  "b1_3"
.else
	b0 =  __mullong_PARM_2
	b1 = (__mullong_PARM_2+1)
	b2 = (__mullong_PARM_2+2)
	b3 = (__mullong_PARM_2+3)

.ifdef __SDCC_NOOVERLAY
	.area DSEG    (DATA)
.else
	.area OSEG    (OVR,DATA)
.endif
__mullong_PARM_2:
	.ds	4
.endif

	.area CSEG    (CODE)

__mullong:
				; parameter a comes in a, b, dph, dpl
	mov	r2,b		; save parameter a
	mov	r3,a

				;	Byte 0
	mov	a,a0
	mov	b,b0
	mul	ab		; a0 * b0
	mov	c0,a
	mov	c1,b

				;	Byte 1
	mov	a,a1
	mov	b,b0
	mul	ab		; a1 * b0
	add	a,c1
	mov	c1,a
	clr	a
	addc	a,b
	mov	c2,a

	mov	a,a0
	mov	b,b1
	mul	ab		; a0 * b1
	add	a,c1
	mov	c1,a
	mov	a,b
	addc	a,c2
	mov	c2,a
	clr	a
	rlc	a
	mov	c3,a

				;	Byte 2
	mov	a,a2
	mov	b,b0
	mul	ab		; a2 * b0
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	a,a1
	mov	b,b1
	mul	ab		; a1 * b1
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	a,a0
	mov	b,b2
	mul	ab		; a0 * b2
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

				;	Byte 3
	mov	a,a3
	mov	b,b0
	mul	ab		; a3 * b0
	add	a,c3
	mov	c3,a

	mov	a,a2
	mov	b,b1
	mul	ab		; a2 * b1
	add	a,c3
	mov	c3,a

	mov	a,a1
	mov	b,b2
	mul	ab		; a1 * b2
	add	a,c3
	mov	c3,a

	mov	a,a0
	mov	b,b3
	mul	ab		; a0 * b3
	add	a,c3

	mov	b,c2
	mov	dph,c1
	mov	dpl,c0
	ret

