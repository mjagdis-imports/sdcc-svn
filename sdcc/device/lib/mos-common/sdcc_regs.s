;-------------------------------------------------------------------------
;   sdcc_regs.s - virtual registers used for mos6502 codegen
;
;   Copyright (C) 2023-2026, Gabriele Gorla
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

	.module sdcc_regs
	.include "config.inc"

;--------------------------------------------------------
; exported ZP symbols
;--------------------------------------------------------

; Keep this in sync with NUM_TEMP_REGS in mos6502/gen.c
; virtual registers used by SDCC codegen
; REGTEMP stack for temporaries
; DPTR used for dereferencing pointers
; sdcc_vregX used for return values of size>2

	.area	ZP (PAG)
REGTEMP::
sdcc_vreg0::
        .ds 1
sdcc_vreg1::
        .ds 1
sdcc_vreg2::
        .ds 1
sdcc_vreg3::
        .ds 1
sdcc_vreg4::
        .ds 1
sdcc_vreg5::
        .ds 1
DPTR::
sdcc_vreg6::
        .ds 1
sdcc_vreg7::
        .ds 1
