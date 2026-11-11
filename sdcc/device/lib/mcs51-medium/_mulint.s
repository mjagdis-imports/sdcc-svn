;-------------------------------------------------------------------------
;  _mulint.s - routine for (unsigned) int (16 bit) multiplication
;
;  Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
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
;--------------------------------------------------------------------------

	.module _mulint
	.include "config.inc"

; Signed and unsigned multiplication are the same - as long as the output
; has the same precision as the input.

	.globl	__mulint
	.globl	__mulint_PARM_2

	.area XSEG    (XDATA)
__mulint_PARM_2:
	.ds 2

	.area CSEG    (CODE)

; int _mulint (int a, int b) __SDCC_NONBANKED

; mulint=
;     (int)(lsb_a*lsb_b) +
;     (char)(msb_a*lsb_b)<<8 +
;     (char)(lsb_a*msb_b)<<8

__mulint:
    mov r2,dph ; msb_a
    mov r3,dpl ; lsb_a

    mov b,r3 ; lsb_a
.ifdef __SDCC_PARMS_IN_BANK1
    mov a,b1_0
.else
    mov dptr,#__mulint_PARM_2
    movx a,@dptr ; lsb_b
.endif
    mul ab ; lsb_a*lsb_b
    mov r0,a
    mov r1,b

    mov b,r2 ; msb_a
.ifdef __SDCC_PARMS_IN_BANK1
    mov a,b1_0
.else
    movx a,@dptr ; lsb_b
.endif
    mul ab ; msb_a*lsb_b
    add a,r1
    mov r1,a

    mov b,r3 ; lsb_a
.ifdef __SDCC_PARMS_IN_BANK1
    mov a,b1_1
.else
    inc dptr
    movx a,@dptr ; msb_b
.endif
    mul ab ; lsb_a*msb_b
    add a,r1

    mov dph,a
    mov dpl,r0
    ret

