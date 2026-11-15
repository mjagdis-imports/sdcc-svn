;-------------------------------------------------------------------------
;  _fslt.s - Floating point library in optimized assembly for 8051
;
;  Copyright (c) 2004, Paul Stoffregen, paul@pjrc.com
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
;-------------------------------------------------------------------------

	.module _fslt
	.include "config.inc"

	.globl	___fslt

	.area CSEG (CODE)

; _Bool __fslt (float a, float b)

___fslt:
	mov	r7, a
	mov	r0, sp
	dec	r0
	dec	r0
	lcall	fs_check_negative_zeros
	setb	sign_a
	rlc	a
	mov	a, @r0
	jc	a_negative
a_positive:
	jnb	acc.7, ab_positive
	; a is positive and b is negative, so a > b
	mov	dpl, #0
	ret
a_negative:
	jb	acc.7, ab_negative
	; a is negative and b is positive, so a < b
	mov	dpl, #1
	ret
ab_positive:
	clr	sign_a
ab_negative:
	lcall	fs_compare_uint32
	mov	a, r1
	jnz	ab_different
	; a and b are equal
	mov	dpl, a
	ret
ab_different:
	jb	sign_a, skip_invert
	cpl	c
skip_invert:
	clr	a
	rlc	a
	mov	dpl, a
	ret

