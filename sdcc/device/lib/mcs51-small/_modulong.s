;-------------------------------------------------------------------------
;  _modulong.s - routine for modulus of 32 bit unsigned long
;
;  Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
;  Bug fixes by Martijn van Balen, aed@iae.nl
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

	.module _modulong
	.include "config.inc"

; Signed and unsigned multiplication are the same - as long as the output
; has the same precision as the input.

	.globl __modulong

	.define a0	"dpl"
	.define a1	"dph"
	.define a2	"b"
	.define a3	"r1"
	.define count   "r0"


.ifdef __SDCC_PARMS_IN_BANK1
	.define b0      "b1_0"
	.define b1      "b1_1"
	.define b2      "b1_2"
	.define b3      "b1_3"
.else
	.globl __modulong_PARM_2
	.globl __modslong_PARM_2

	.define b0      "__modulong_PARM_2"
	.define b1      "__modulong_PARM_2 + 1"
	.define b2      "__modulong_PARM_2 + 2"
	.define b3      "__modulong_PARM_2 + 3"

.ifdef __SDCC_NOOVERLAY
	.area DSEG    (DATA)
.else
	.area OSEG    (OVR,DATA)
.endif

__modulong_PARM_2:
__modslong_PARM_2:
	.ds	4
.endif

	.area CSEG    (CODE)

__modulong:				; parameter a comes in a, b, dph, dpl
	mov	a3,a		; save parameter a3

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
	mov	r4,a
	mov	a,a1
	subb	a,b1
	mov	r5,a
	mov	a,a2
	subb	a,b2
	mov	r6,a
	mov	a,a3
	subb	a,b3

	jc	smaller		; a >= b?

	mov	a3,a		; -> yes;  a = a - b;
	mov	a2,r6
	mov	a1,r5
	mov	a0,r4
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

