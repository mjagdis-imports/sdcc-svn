;-------------------------------------------------------------------------
;   __mulint.c - routine for (unsigned) int (16 bit) multiplication
;
;   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
;   Copyright (C) 2026, Gabriele Gorla
;
;   This library is free software; you can redistribute it and/or modify it
;   under the terms of the GNU General Public License as published by the
;   Free Software Foundation; either version 2, or (at your option) any
;   later version.
;
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public License
;   along with this library; see the file COPYING. If not, write to the
;   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;   As a special exception, if you link this library with other files,
;   some of which are compiled with SDCC, to produce an executable,
;   this library does not by itself cause the resulting executable to
;   be covered by the GNU General Public License. This exception does
;   not however invalidate any other reasons why the executable file
;   might be covered by the GNU General Public License.
;-------------------------------------------------------------------------

	.module __mulint
	.include "config.inc"

	.globl ___mulint


;--------------------------------------------------------
;int __mulint (int a, int b)
;--------------------------------------------------------

.area CSEG

___mulint:
   	ais #-2
	psha
	pshx

	ldx 8,s
	mul
	sta 4,s
	stx 3,s

	lda 1,s
	ldx 8,s
	mul
	add 3,s
	sta 3,s

	lda 2,s
	ldx 7,s
	mul
	add 3,s
	sta 3,s

	ais #2
	pulx
	pula
	rts

