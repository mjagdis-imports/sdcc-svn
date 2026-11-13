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

	.define b0 "r1"

	; c0  a0 * b0
	; c1  a1 * b0 + a0 * b1
	; c2  a2 * b0 + a1 * b1 + a0 * b2
	; c3  a3 * b0 + a2 * b1 + a1 * b2 + a0 * b3


	.area PSEG    (PAG,XDATA)

__mullong_PARM_2:
	.ds	4

	.area CSEG    (CODE)

__mullong:
				; parameter a comes in a, b, dph, dpl
	mov	r2,b		; save parameter a
	mov	r3,a

.ifdef __SDCC_USE_XSTACK
	mov	a,#-4		;  1  b 4 bytes
	add	a,_spx		;  1
	mov	r0,a		;  1  r0 points to b0
.else
;if defined (__SDCC_MODEL_MEDIUM)
	mov	r0,#__mullong_PARM_2
.endif

				;	Byte 0
	movx	a,@r0		; b0
	mov	b0,a		; we need b0 several times
	inc	r0		; r0 points to b1
	mov	b,a0
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

	mov	b,a0
	movx	a,@r0		; b1
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

	mov	b,a1
	movx	a,@r0		; b1
	mul	ab		; a1 * b1
	add	a,c2
	mov	c2,a
	mov	a,b
	addc	a,c3
	mov	c3,a

	mov	b,a0
	inc	r0
	movx	a,@r0		; b2
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

	mov	b,a1
	movx	a,@r0		; b2
	mul	ab		; a1 * b2
	add	a,c3
	mov	c3,a

	mov	b,a2
	dec	r0
	movx	a,@r0		; b1
	mul	ab		; a2 * b1
	add	a,c3
	mov	c3,a

	mov	b,a0
	inc	r0
	inc	r0
	movx	a,@r0		; b3
	mul	ab		; a0 * b3
	add	a,c3

	mov	b,c2
	mov	dph,c1
	mov	dpl,c0
	ret

