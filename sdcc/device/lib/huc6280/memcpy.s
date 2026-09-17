;-------------------------------------------------------------------------
;   memcpy.s - standarc C library
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

	.module _memcpy
	.huc6280

;--------------------------------------------------------
; exported symbols
;--------------------------------------------------------
	.globl ___memcpy_PARM_2
	.globl ___memcpy_PARM_3
	.globl ___memcpy
	.globl _memcpy_PARM_2
	.globl _memcpy_PARM_3
	.globl _memcpy

;--------------------------------------------------------
; local aliases
;--------------------------------------------------------
;	.define dst   "DPTR"
	.define src   "___memcpy_PARM_2"
	.define count "___memcpy_PARM_3"


	.area DATA
TII_memcpy:
	.db	0x73	; TII
_memcpy_PARM_2:
___memcpy_PARM_2:
	.dw	0x0000
dst:
	.dw	0x0000
_memcpy_PARM_3:
___memcpy_PARM_3:
	.dw	0x0000
	.dw	0x60	; RTS

;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE

_memcpy:
___memcpy:
	sta	dst+0
	lda count+0
	ora count+1
	beq end		; len == 0
	stx dst+1
	jsr TII_memcpy

end:
	lda	dst+0
	rts
