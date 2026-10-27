;-------------------------------------------------------------------------
;   cosf.s - routine for floating point cosine
;
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

	.module cosf
	
;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl _cosf

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

_cosf:
;	if (x==0.0) return 1.0;
	lda	*(_cosf_PARM_1+3)
	and	#0x7F
	ora	*(_cosf_PARM_1+2)
	ora	*(_cosf_PARM_1+1)
	ora	*_cosf_PARM_1
	bne	not_zero

;	return 1
;	lda	#0x00
	tax
	ldy	#0x80
	sty	*___SDCC_m6502_ret2
	ldy	#0x3f
	sty	*___SDCC_m6502_ret3
	rts

not_zero:
;	return sincosf(x, 1);
	lda	*(_cosf_PARM_1+3)
	sta	(_sincosf_PARM_1+3)
	lda	*(_cosf_PARM_1+2)
	sta	(_sincosf_PARM_1+2)
	lda	*(_cosf_PARM_1+1)
	sta	(_sincosf_PARM_1+1)
	lda	*_cosf_PARM_1
	sta	_sincosf_PARM_1

	ldx	#0x01
	stx	_sincosf_PARM_2
	jmp	_sincosf

