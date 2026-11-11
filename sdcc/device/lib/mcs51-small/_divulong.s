;-------------------------------------------------------------------------
;  _divulong.s - routine for division of 32 bit unsigned long
;
;  Copyright (C) 1999, Jean-Louis Vern <jlvern AT gmail.com>
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

; Signed and unsigned multiplication are the same - as long as the output
; has the same precision as the input.

	.globl __divulong

	.define count   "r2"

	.define x0	"dpl"
	.define x1	"dph"
	.define x2	"b"
	.define x3	"r3"

	.define reste0	"r4"
	.define reste1	"r5"
	.define reste2	"r6"
	.define reste3	"r7"

.ifdef __SDCC_PARMS_IN_BANK1
	.define y0      "b1_0"
	.define y1      "b1_1"
	.define y2      "b1_2"
	.define y3      "b1_3"
.else
	.globl __divulong_PARM_2
	.globl __divslong_PARM_2

	.define y0      "__divulong_PARM_2"
	.define y1      "__divulong_PARM_2 + 1"
	.define y2      "__divulong_PARM_2 + 2"
	.define y3      "__divulong_PARM_2 + 3"

.ifdef __SDCC_NOOVERLAY
	.area DSEG    (DATA)
.else
	.area OSEG    (OVR,DATA)
.endif

__divulong_PARM_2:
__divslong_PARM_2:
	.ds	4
.endif

	.area CSEG    (CODE)

__divulong:
		; parameter x comes in a, b, dph, dpl
	mov	x3,a		; save parameter x3

	mov	count,#32
	clr	a
	mov	reste0,a
	mov	reste1,a
	mov	reste2,a
	mov	reste3,a

	; optimization  loop in lp0 until the first bit is shifted into rest

lp0:	mov	a,x0		; x <<= 1
	add	a,x0
	mov	x0,a
	mov	a,x1
	rlc	a
	mov	x1,a
	mov	a,x2
	rlc	a
	mov	x2,a
	mov	a,x3
	rlc	a
	mov	x3,a

	jc	in_lp
	djnz	count,lp0

	sjmp	exit

loop:	mov	a,x0		; x <<= 1
	add	a,x0
	mov	x0,a
	mov	a,x1
	rlc	a
	mov	x1,a
	mov	a,x2
	rlc	a
	mov	x2,a
	mov	a,x3
	rlc	a
	mov	x3,a

in_lp:	mov	a,reste0	; reste <<= 1
	rlc	a		; feed in carry
	mov	reste0,a
	mov	a,reste1
	rlc	a
	mov	reste1,a
	mov	a,reste2
	rlc	a
	mov	reste2,a
	mov	a,reste3
	rlc	a
	mov	reste3,a

	mov	a,reste0	; reste - y
	subb	a,y0		; carry is always clear here, because
					; reste <<= 1 never overflows
	mov	a,reste1
	subb	a,y1
	mov	a,reste2
	subb	a,y2
	mov	a,reste3
	subb	a,y3

	jc	minus		; reste >= y?

				; -> yes;  reste -= y;
	mov	a,reste0
	subb	a,y0		; carry is always clear here (jc)
	mov	reste0,a
	mov	a,reste1
	subb	a,y1
	mov	reste1,a
	mov	a,reste2
	subb	a,y2
	mov	reste2,a
	mov	a,reste3
	subb	a,y3
	mov	reste3,a

	orl	x0,#1

minus:	djnz	count,loop	; -> no

exit:	mov	a,x3		; prepare the return value
	ret

