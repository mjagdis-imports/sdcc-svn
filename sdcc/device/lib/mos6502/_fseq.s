;-------------------------------------------------------------------------
;   _fseq.s - routine for floating point comparison
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

	.module _fseq

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl ___fseq

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

___fseq:
	lda	*___fseq_PARM_1
	cmp	*___fseq_PARM_2
	bne	neq1
	lda	*(___fseq_PARM_1+1)
	cmp	*(___fseq_PARM_2+1)
	bne	neq1
	lda	*(___fseq_PARM_1+2)
	cmp	*(___fseq_PARM_2+2)
	bne	neq1
	lda	*(___fseq_PARM_1+3)
	cmp	*(___fseq_PARM_2+3)
	bne	neq1
ret1:
	lda	#0x01
	rts

neq1:
	lda	*___fseq_PARM_1
	ora	*___fseq_PARM_2
	bne	ret0
	lda	*(___fseq_PARM_1+1)
	ora	*(___fseq_PARM_2+1)
	bne	ret0
	lda	*(___fseq_PARM_1+2)
	ora	*(___fseq_PARM_2+2)
	bne	ret0
	lda	*(___fseq_PARM_1+3)
	ora	*(___fseq_PARM_2+3)
	and	#0x7f
	beq ret1
ret0:
	lda #0x00
	rts
