;-------------------------------------------------------------------------
;   _fslt.s - routine for floating point comparison
;
;   Copyright (C) 2025-2026, Gabriele Gorla
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

	.module _fslt

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl ___fslt

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

___fslt:
	lda	*(___fslt_PARM_1 + 3)
	ora	*(___fslt_PARM_2 + 3)
        and	#0x7f
	bne	not_zero
	ora 	*(___fslt_PARM_1 + 2)
	ora 	*(___fslt_PARM_2 + 2)
;	bne	not_zero
	ora	*(___fslt_PARM_1 + 1)
	ora	*(___fslt_PARM_2 + 1)
	ora	*(___fslt_PARM_1 + 0)
	ora	*(___fslt_PARM_2 + 0)
	beq 	reta
not_zero:
	lda	*(___fslt_PARM_1 + 3)
	eor	*(___fslt_PARM_2 + 3)
	bpl	same_sign
	lda	*(___fslt_PARM_1 + 3)
rets:
	bpl	ret0
ret1:
	lda	#0x01
reta:
	rts

same_sign:
	sec
	lda	*(___fslt_PARM_1 + 3)
	bpl	both_pos

; both neg
	lda	*(___fslt_PARM_2 + 3)
	sbc	*(___fslt_PARM_1 + 3)
	bne	retv
	sec
	lda	*___fslt_PARM_2
	sbc	*___fslt_PARM_1
	lda	*(___fslt_PARM_2 + 1)
	sbc	*(___fslt_PARM_1 + 1)
	lda	*(___fslt_PARM_2 + 2)
	sbc	*(___fslt_PARM_1 + 2)
	lda	*(___fslt_PARM_2 + 3)
	sbc	*(___fslt_PARM_1 + 3)
retv:
	bvc	rets
	bpl	ret1
ret0:
	lda	#0x00
	rts

both_pos:
;	lda	*(___fslt_PARM_1 + 3)
	sbc	*(___fslt_PARM_2 + 3)
	bne	retv
	sec
	lda	*___fslt_PARM_1
	sbc	*___fslt_PARM_2
	lda	*(___fslt_PARM_1 + 1)
	sbc	*(___fslt_PARM_2 + 1)
	lda	*(___fslt_PARM_1 + 2)
	sbc	*(___fslt_PARM_2 + 2)
	lda	*(___fslt_PARM_1 + 3)
	sbc	*(___fslt_PARM_2 + 3)
	jmp	retv

