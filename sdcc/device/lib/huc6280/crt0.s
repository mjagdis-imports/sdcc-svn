;--------------------------------------------------------------------------
;  crt0.s - Generic crt0.s for a bare metal huc6280
;
;  Copyright (C) 2021-2026, Gabriele Gorla
;  Copyright (C) 2023, Maarten Brock
;
;  This library is free software; you can redistribute it and/or modify it
;  under the terms of the GNU General Public License as published by the
;  Free Software Foundation; either version 2, or (at your option) any
;  later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with this library; see the file COPYING. If not, write to the
;  Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;  As a special exception, if you link this library with other files,
;  some of which are compiled with SDCC, to produce an executable,
;  this library does not by itself cause the resulting executable to
;  be covered by the GNU General Public License. This exception does
;  not however invalidate any other reasons why the executable file
;   might be covered by the GNU General Public License.
;--------------------------------------------------------------------------

	.module crt0
	.huc6280

;--------------------------------------------------------
;  Ordering of segments for the linker.
;--------------------------------------------------------
	.area ZP      (PAG)
	.area OSEG    (PAG, OVR)

	.area _DATA
	.area DATA
	.area BSS

	.area _CODE
	.area GSINIT
	.area GSFINAL
	.area CODE
	.area RODATA
	.area XINIT

;--------------------------------------------------------
;  Memory mapped register init location
;  Must be in the range 0x0000-0x1ff5
;--------------------------------------------------------
__mmap_init = 0x1f00

;--------------------------------------------------------
;  Reset/interrupt vectors
;--------------------------------------------------------
	.area CODEIVT (ABS)
	.org  0x1ff6
	.dw	__sdcc_gs_init_startup ; IRQ2 (EXT/BRK)
	.dw	__sdcc_gs_init_startup ; IRQ1 (VDC)
	.dw	__sdcc_gs_init_startup ; TIMER
	.dw	__sdcc_gs_init_startup ; NMI
	.dw	__mmap_init|0xe000     ; RESET
 
    .area MMAP_INIT (ABS)
	.org __mmap_init
	lda	#0xff
	tam0		; 0x0000-0x1fff <- 0x1fe000-0x1fffff
	lda	#0x01 	; should be #0xf8 DATA (8k)
	tam1		; 0x2000-0x3fff <- 0x1f0000-0x1f1fff
	lda	#0x02 	; should be start of CODE
	tam2		; 0x4000-0x5fff <- 0x004000-0x005fff
	inc a
	tam3		; 0x6000-0x7fff <- 0x006000-0x007fff
	inc a
	tam4		; 0x8000-0x9fff <- 0x008000-0x009fff
	inc a
	tam5		; 0xa000-0xbfff <- 0x00a000-0x00bfff
	inc a
	tam6		; 0xc000-0xdfff <- 0x00c000-0x00dfff
; MSR7 is set to 0 at reset
				; 0xe000-0xffff <- 0x000000-0x001fff
    jmp __sdcc_gs_init_startup

;--------------------------------------------------------
;  Startup Code
;--------------------------------------------------------
	.area GSINIT
__sdcc_gs_init_startup:
	ldx	#0xff
	txs
;	ldx	#0x21         ; MSB of stack ptr
;	stx	__BASEPTR+1

;; Skip initialisation of global variables if __sdcc_external_startup
;; returned non-zero value.
	jsr	___sdcc_external_startup
	ora	#0
	beq	__sdcc_init_data
	jmp	__sdcc_program_startup

__sdcc_init_data:
; clear ZP
	lda	#0x00
	ldx	#<s_ZP
	ldy	#<l_ZP
	beq	skipZP
ZPloop:
	sta	*0,x
	inx
	dey
	bne	ZPloop
skipZP:

; initialize DATA
	lda	# (<l_XINIT)
	ora # (>l_XINIT)
	beq	skip
	tii s_XINIT, s_DATA, l_XINIT
skip:

; clear BSS
	lda	#>l_BSS
	sta	*_memset_PARM_3+1
	lda	#<l_BSS
	sta	*_memset_PARM_3
	lda	#0x00
	sta	*_memset_PARM_2
	lda	#<s_BSS
	ldx	#>s_BSS
	jsr	_memset

;--------------------------------------------------------
;  Final Cleanup
;--------------------------------------------------------
	.area GSFINAL
__sdcc_program_startup:
	jsr	_main
	jmp	.

