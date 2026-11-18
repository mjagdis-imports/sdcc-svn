;-------------------------------------------------------------------------
;   __divsint.s - routine for division of 16 bit signed int
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

	.module __divsint
	.include "config.inc"

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl ___divsint
	.globl ___sdivmod16

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define res "sdcc_vreg0"
	.define den "___divsint_PARM_2"
	.define rem "sdcc_vreg2"
	.define s1  "sdcc_vreg4"
	.define s2  "sdcc_vreg5"

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

___divsint:
	jsr	___sdivmod16
	ldx	*res+1
	lda	*s1
	eor	*s2
	bpl	pos
	lda	*res+0
	jmp	___negax
pos:
	lda	*res+0
	rts

___sdivmod16:
	stx	*s1
	jsr	_abs
	tay
	lda	*___divsint_PARM_2+1
	sta	*s2
	bpl	skip
	sec
	CLA
	sbc	*___divsint_PARM_2+0
        sta	*___divsint_PARM_2+0
	CLA
	sbc	*___divsint_PARM_2+1
        sta	*___divsint_PARM_2+1
skip:
	tya
	jmp	___udivmod16

