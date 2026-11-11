;-------------------------------------------------------------------------
;  _modulong.c - routine for modulus of 32 bit unsigned long
;
;  Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
;  Bug fixes by Martijn van Balen, aed@iae.nl
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

	.module _divulong
	.include "config.inc"

	.globl __modulong
	.globl __modlong

	.define a0	"dpl"
	.define a1	"dph"
	.define a2	"b"
	.define a3	"r1"

	.define b0	"r2"
	.define b1	"r3"
	.define b2	"r4"
	.define b3	"r5"

	.define count   "r0"

	ar2 = 2			; BUG register set is not considered
	ar3 = 3
	ar4 = 4
	ar5 = 5

	.area	CSEG (CODE)

__modulong:
				; parameter a comes in a, b, dph, dpl
	mov	a3,a		; save parameter a3

	mov	a,sp
	add	a,#-2-3		; 2 bytes return address, 3 bytes param b
	mov	r0,a		; r1 points to b0

	mov	ar2,@r0		; load b0
	inc	r0		; r0 points to b1
	mov	ar3,@r0		; b1
	inc	r0
	mov	ar4,@r0		; b2
	inc	r0
	mov	ar5,@r0		; b3

__modlong:			; entry point for __modslong
				; a in r1, b, dph, dpl
				; b in r5, r4, r3, r2

	mov	count,#0

	mov	a,b0		; b == 0? avoid endless loop
	orl	a,b1
	orl	a,b2
	orl	a,b3
	jz	div_by_0

	mov	count,#0
	clr	c		; when loop1 jumps immediately to loop2

loop1:	inc	count

	mov	a,b3		; if (!MSB_SET(b))
	jb	acc.7,loop2

	mov	a,b0		; b <<= 1
	add	a,acc
	mov	b0,a
	mov	a,b1
	rlc	a
	mov	b1,a
	mov	a,b2
	rlc	a
	mov	b2,a
	mov	a,b3
	rlc	a
	mov	b3,a

	mov	a,a0		; a - b
	subb	a,b0		; here carry is always clear
	mov	a,a1
	subb	a,b1
	mov	a,a2
	subb	a,b2
	mov	a,a3
	subb	a,b3

	jnc	loop1

	clr	c
	mov	a,b3		; b >>= 1;
	rrc	a
	mov	b3,a
	mov	a,b2
	rrc	a
	mov	b2,a
	mov	a,b1
	rrc	a
	mov	b1,a
	mov	a,b0
	rrc	a
	mov	b0,a

loop2:	; clr	c		  never set
	mov	a,a0		; a - b
	subb	a,b0
	mov	a,a1
	subb	a,b1
	mov	r6,a		; d1
	mov	a,a2
	subb	a,b2
	mov	r7,a		; d2
	mov	a,a3
	subb	a,b3

	jc	smaller		; a >= b?

	mov	a3,a		; -> yes;  a = a - b;
	mov	a2,r7
	mov	a1,r6
	mov	a,a0
	subb	a,b0
	mov	a0,a
smaller:			; -> no
	clr	c
	mov	a,b3		; b >>= 1;
	rrc	a
	mov	b3,a
	mov	a,b2
	rrc	a
	mov	b2,a
	mov	a,b1
	rrc	a
	mov	b1,a
	mov	a,b0
	rrc	a
	mov	b0,a

	djnz	count,loop2

	mov	a,a3		; prepare the return value
div_by_0:
	ret

