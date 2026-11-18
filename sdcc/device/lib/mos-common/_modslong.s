;-------------------------------------------------------------------------
;   __modslong.s - routine for remainder for 32 bit signed long division
;
;   Copyright (C) 1998, Ullrich von Bassewitz
;   Copyright (C) 2022-2026, Gabriele Gorla
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

	.module __modslong

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl ___modslong
	.include "config.inc"

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define res0 "___divslong_PARM_1+0"
	.define res1 "___divslong_PARM_1+1"
	.define res2 "sdcc_vreg2"
	.define res3 "sdcc_vreg3"
	.define den  "___divslong_PARM_2"
	.define rem  "sdcc_vreg4"
	.define s1   "sdcc_vreg0"
	.define s2   "sdcc_vreg1"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

___modslong:
	jsr	___sdivmod32
	lda	*s1
	bpl	pos
; neg res
	sec
	CLA
	sbc	*rem+0
	tay
	CLA
	sbc	*rem+1
	tax
	CLA
	sbc	*rem+2
	sta	*res2
	CLA
	sbc	*rem+3
	sta	*res3
	tya
	rts
pos:
	lda	*rem+3
	sta	*res3
	lda	*rem+2
	sta	*res2
	ldx	*rem+1
	lda	*rem+0
	rts

