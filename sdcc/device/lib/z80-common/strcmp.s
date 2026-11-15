;--------------------------------------------------------------------------
;  strcmp.s
;
;  Copyright (c) 2026, Philipp Klaus Krause
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

	.module strcmp
	.include "config.inc"

	.area   _CODE

	.globl _strcmp

_strcmp:
	ld	c, l
	ld	b, h
	ex	de, hl
loop:
	ld	a, (bc)
	sub	a, (hl)
	ld	e, a
	sbc	a, a
	ld	d, a
	or	a, e
	ret	nz
	ld	a, (hl)
	or	a, a
	ret	z
	inc	hl
	inc	bc
	jp	loop

