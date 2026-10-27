;-------------------------------------------------------------------------
;   sinf.s - routine for floating point sine
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

	.module sinf

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl _sinf

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

_sinf:
	lda	*(_sinf_PARM_1+3)
	and	#0x7F
	ora	*(_sinf_PARM_1+2)
	ora	*(_sinf_PARM_1+1)
	ora	*_sinf_PARM_1
	bne	not_zero
;	tax
;	sta	*___SDCC_m6502_ret2
;	sta	*___SDCC_m6502_ret3
;	rts
	jmp ___fs_ret_zero

not_zero:
;	../sinf.c: 39: return sincosf(x, 0);
	lda	*(_sinf_PARM_1+3)
	sta	(_sincosf_PARM_1+3)
	lda	*(_sinf_PARM_1+2)
	sta	(_sincosf_PARM_1+2)
	lda	*(_sinf_PARM_1+1)
	sta	(_sincosf_PARM_1+1)
	lda	*_sinf_PARM_1
	sta	_sincosf_PARM_1

	ldy	#0x00
	sty	_sincosf_PARM_2

	jmp	_sincosf

