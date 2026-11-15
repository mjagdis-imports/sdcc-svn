;-------------------------------------------------------------------------
;  _fs2ulong.s - Floating point library in optimized assembly for 8051
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

	.module _fs2ulong
	.include "config.inc"

	.globl	___fs2ulong
	.globl	fs2ulong_begin

	.area CSEG (CODE)

; unsigned long __fs2ulong (float x)

___fs2ulong:
	mov	r7, #158

fs2ulong_begin:
	lcall	fsgetarg
	jnb	sign_a, fs2ulong_int
	ljmp	fs_return_zero

fs2ulong_int:
	clr	c
	mov	a, r7
	subb	a, exp_a
	jnc	fs2ulong_int_ok
	; if we get here, x >= 2^32
	mov	a, #0xFF
	mov	b, a
	mov	dph, a
	mov	dpl, a
	ret

fs2ulong_int_ok:
	mov	r1, #0
	lcall	fs_rshift_a

fs2ulong_done:
	mov	dpl, r1
	mov	dph, r2
	mov	b, r3
	mov	a, r4
	ret

