;-------------------------------------------------------------------------
;   __divulong.s - routine for 32 bit unsigned long division
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

	.module __divulong
	.include "config.inc"

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl ___divulong_PARM_1
	.globl ___divulong_PARM_2
	.globl ___divslong_PARM_1
	.globl ___divslong_PARM_2
	.globl ___modulong_PARM_1
	.globl ___modulong_PARM_2
	.globl ___modslong_PARM_1
	.globl ___modslong_PARM_2
	.globl _ldiv_PARM_1
	.globl _ldiv_PARM_2
	.globl ___divulong
	.globl ___udivmod32

;--------------------------------------------------------
; overlayable function parameters in zero page
;--------------------------------------------------------
	.area	OSEG    (PAG, OVR)
___divulong_PARM_1:
___divslong_PARM_1:
___modulong_PARM_1:
___modslong_PARM_1:
_ldiv_PARM_1:
	.ds 4
___divulong_PARM_2:
___divslong_PARM_2:
___modulong_PARM_2:
___modslong_PARM_2:
_ldiv_PARM_2:
	.ds 4

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
	.define res0 "___divulong_PARM_1+0"
	.define res1 "___divulong_PARM_1+1"
	.define res2 "sdcc_vreg2"
	.define res3 "sdcc_vreg3"
	.define den  "___divulong_PARM_2"
	.define rem  "sdcc_vreg4"
	
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

___divulong:
	jsr	___udivmod32
	lda	*res0
	ldx	*res1
	rts

___udivmod32:
        ldx	*___divulong_PARM_1+2
        stx	*res2
        ldx	*___divulong_PARM_1+3
        stx	*res3

	CLA
        sta     *rem+0
        sta     *rem+1
        sta     *rem+2
        sta     *rem+3
        ldy     #32
L0:
	asl     *res0
        rol     *res1
        rol     *res2
        rol     *res3
        rol     a
        rol     *rem+1
        rol     *rem+2
        rol     *rem+3

; Do a subtraction. we do not have enough space to store the intermediate
; result, so we may have to do the subtraction twice.
        tax
        cmp     *den+0
        lda     *rem+1
        sbc     *den+1
        lda     *rem+2
        sbc     *den+2
        lda     *rem+3
        sbc     *den+3
        bcc     L1

; Overflow, do the subtraction again, this time store the result
        sta     *rem+3	; We have the high byte already
        txa
        sbc     *den+0	; byte 0
        tax
        lda     *rem+1
        sbc     *den+1
        sta     *rem+1	; byte 1
        lda     *rem+2
        sbc     *den+2
        sta     *rem+2 	; byte 2
        inc     *res0	; Set result bit
L1:
	txa
        dey
        bne     L0
        sta     *rem+0
        rts

