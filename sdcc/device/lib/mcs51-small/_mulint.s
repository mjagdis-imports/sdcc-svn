;-------------------------------------------------------------------------
;  _mulint.s - routine for (unsigned) int (16 bit) multiplication
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

	.module _mulint
	.include "config.inc"

; Signed and unsigned multiplication are the same - as long as the output
; has the same precision as the input.

	.globl	__mulint
	.globl __mulint_PARM_2

.ifdef __SDCC_PARMS_IN_BANK1
	.define bl 	"b1_0"
	.define bh 	"b1_1"
.else
	.define bl 	"__mulint_PARM_2"
	.define bh 	"__mulint_PARM_2 + 1"
.ifdef __SDCC_NOOVERLAY
	.area DSEG    (DATA)
.else
	.area OSEG    (OVR,DATA)
.endif
__mulint_PARM_2:
	.ds	2
.endif

; int _mulint (int a, int b) __SDCC_NONBANKED

; mulint=
;     (int)(lsb_a*lsb_b) +
;     (char)(msb_a*lsb_b)<<8 +
;     (char)(lsb_a*msb_b)<<8

	.area CSEG    (CODE)

__mulint:
	; clobbered registers none

	mov	a,dpl			;  1  al
	mov	b,bl			;  2  bl
	mul	ab			;  4  al * bl
	xch	a,dpl			;  1  store low-byte of return value, fetch al
	push	b			;  2

	mov	b,bh			;  2  bh
	mul	ab			;  4  al * bh
	pop	b			;  2
	add	a,b			;  1
	xch	a,dph			;  1  ah -> acc

	mov	b,bl			;  2  bl
	mul	ab			;  4  ah * bl
	add	a,dph			;  1
	mov	dph,a			;  1
	ret				;  2
					; 30

