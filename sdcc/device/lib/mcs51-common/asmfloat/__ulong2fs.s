;-------------------------------------------------------------------------
;  _ulong2fs.s - Floating point library in optimized assembly for 8051
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

	.module _ulong2fs
	.include "config.inc"

	.globl	___ulong2fs
	.globl	ulong2fs_doit

	.area CSEG (CODE)

; float long __ulong2fs (unsigned long x)

___ulong2fs:
	mov	r4, a
	mov	r3, b
	mov	r2, dph
	mov	r1, dpl
	mov	a, #158
ulong2fs_doit:
	clr	sign_a
long2fs_doit:
	mov	exp_a, a
	lcall	fs_normalize_a
	ljmp	fs_round_and_return

