;-------------------------------------------------------------------------
;  _fsadd.s - Floating point library in optimized assembly for 8051
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

	.module _fsadd
	.include "config.inc"

	.globl	___fsadd

	.area CSEG (CODE)

; float __fsadd (float a, float b) __reentrant

;	 extract the two inputs, placing them into:
;	      sign     exponent   mantissa
;	      ----     --------   --------
;	  a:  sign_a   exp_a      r4/r3/r2
;	  b:  sign_b   exp_b      r7/r6/r5
;	
;	 r1: used to extend precision of a's mantissa
;	 r0: general purpose loop counter


___fsadd:
	lcall	fsgetargs

	.globl	fsadd_direct_entry
fsadd_direct_entry:
	; we're going to extend mantissa to 32 bits temporarily
	mov	r1, #0

	; which exponent is greater?
	mov	a, exp_b
	cjne	a, exp_a, 00005$
	sjmp	00011$
00005$:	jnc	00010$

	; a's exponent was greater, so shift b's mantissa
	lcall	fs_swap_a_b

00010$:
	; b's exponent was greater, so shift a's mantissa
	mov	a, exp_b
	clr	c
	subb	a, exp_a
	lcall	fs_rshift_a	; acc has # of shifts to do

00011$:
	; decide if we need to add or subtract
	; sign_a and sign_b are stored in the flag bits of psw,
	; so this little trick checks if the arguments have the
	; same sign.
	mov	a, psw
	swap	a
	xrl	a, psw
	jb	acc.1, 00022$

00020$:
	; add the mantissas (both positive or both negative)
	mov	a, r2
	add	a, r5
	mov	r2, a
	mov	a, r3
	addc	a, r6
	mov	r3, a
	mov	a, r4
	addc	a, r7
	mov	r4, a
	; check for overflow past 24 bits
	jnc	00021$
	mov	a, #1
	lcall	fs_rshift_a
	mov	a, r4
	orl	a, #0x80
	mov	r4, a
00021$:
	ljmp	fs_round_and_return



00022$:
	; subtract the mantissas (one of them is negative)
	clr	c
	mov	a, r2
	subb	a, r5
	mov	r2, a
	mov	a, r3
	subb	a, r6
	mov	r3, a
	mov	a, r4
	subb	a, r7
	mov	r4, a
	jnc	00025$
	; if we get a negative result, turn it positive and
	; flip the sign bit
	clr	c
	clr	a
	subb	a, r1
	mov	r1, a
	clr	a
	subb	a, r2
	mov	r2, a
	clr	a
	subb	a, r3
	mov	r3, a
	clr	a
	subb	a, r4
	mov	r4, a
	cpl	sign_a
00025$:
	lcall	fs_normalize_a
	ljmp	fs_round_and_return

