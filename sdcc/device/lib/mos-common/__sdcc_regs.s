;-------------------------------------------------------------------------
;   __sdcc_regs.s - SDCC support for indirect JSR 
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

	.module __sdcc_regs
	.include "config.inc"

;--------------------------------------------------------
; exported ZP symbols
;--------------------------------------------------------

; PSEUDO registers used by SDCC codegen
; REGTEMP stack for temporaries
; Keep this in sync with NUM_TEMP_REGS in mos6502/gen.c
; DPTR used for dereferencing pointers
; ___SDCC_m6502_retX used for return values of size>2

	.area	ZP (PAG)
REGTEMP::
___SDCC_m6502_ret0::
        .ds 1
___SDCC_m6502_ret1::
        .ds 1
___SDCC_m6502_ret2::
        .ds 1
___SDCC_m6502_ret3::
        .ds 1
___SDCC_m6502_ret4::
        .ds 1
___SDCC_m6502_ret5::
        .ds 1
DPTR::
___SDCC_m6502_ret6::
        .ds 1
___SDCC_m6502_ret7::
        .ds 1
