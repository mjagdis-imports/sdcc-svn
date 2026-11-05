;--------------------------------------------------------------------------
;  strlen.s - part of string library functions
;
;  Copyright (C) 2007, Frieder Ferlemann (2007)
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

	.module strlen
	.include "config.inc"

	.globl	_strlen

	.area CSEG    (CODE)

; size_t strlen ( const char * str )
      ; dptr holds pointer
      ; b holds pointer memspace
      ;

_strlen:
      ; char *ptr = str:
      mov     r2,dpl
      mov     r3,dph
      ;

      ; while ( *ptr ) ptr++;
    L00101$:
      lcall   __gptrget
      jz      L00102$
      inc     dptr
      sjmp    L00101$
      ;

    L00102$:
      ; return ptr - str;
      clr     c
      mov     a,dpl
      subb    a,r2
      mov     dpl,a
      ;
      mov     a,dph
      subb    a,r3
      mov     dph,a
      ;
      RET
