;-------------------------------------------------------------------------
;  _moduint.s - routine for unsigned int (16 bit) modulus
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

	.module _moduint
	.include "config.inc"

	.globl	__moduint
	.globl __modint
	.globl __moduint_PARM_2
	.globl __modsint_PARM_2

.define al      "dpl"
.define ah      "dph"

.ifdef __SDCC_NOOVERLAY
	.area DSEG    (DATA)
.else
	.area OSEG    (OVR,DATA)
.endif

.ifdef __SDCC_PARMS_IN_BANK1
	.define bl 	"b1_0"
	.define bh 	"b1_1"
.else
	.define bl 	"__moduint_PARM_2"
	.define bh 	"__moduint_PARM_2 + 1"
__moduint_PARM_2:
__modsint_PARM_2:
	.ds	2
.endif

.define count   "r2"

	.area CSEG    (CODE)

__moduint:
__modint:			; entry point for __modsint

	mov	a,bl		; avoid endless loop
	orl	a,bh
	jz	div_by_0

	mov	count,#1

loop1:	mov	a,bl		; b <<= 1
	add	a,acc
	mov	bl,a
	mov	a,bh
	rlc	a
	jc	msbset
	mov	bh,a

	mov	a,al		; a - b
	subb	a,bl		; here carry is always clear
	mov	a,ah
	subb	a,bh

	jc	start

	inc	count
	sjmp	loop1

start:	clr	c
	mov	a,bh		; b >>= 1;
msbset:	rrc	a
	mov	bh,a
	mov	a,bl
	rrc	a
	mov	bl,a

loop2:	clr	c
	mov	a,al		; a - b
	subb	a,bl

	mov	b,a
	mov	a,ah
	subb	a,bh

	jc	smaller		; a >= b?

	mov	ah,a		; -> yes;  a = a - b;
	mov	al,b
smaller:			; -> no
	clr	c
	mov	a,bh		; b >>= 1;
	rrc	a
	mov	bh,a
	mov	a,bl
	rrc	a
	mov	bl,a

	djnz	count,loop2
div_by_0:
	ret

