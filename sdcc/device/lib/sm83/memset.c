/*-------------------------------------------------------------------------
   _memset.c - part of string library functions

   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
   Copyright (C) 2020, Sergey Belyashov sergey.belyashov@gmail.com
   Copyright (C) 2022, Sebastian 'basxto' Riedel
   mcs51 assembler by Frieder Ferlemann (2007)

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#undef memset /* Avoid conflict with builtin memset() in Z80 and some related ports */

#ifdef __SDCC_BROKEN_STRING_FUNCTIONS
void *memset (void *s, unsigned char c, size_t n)
#else
void *memset (void *s, int c, size_t n)
#endif

__naked
{
	(void)s;//de
	(void)c;//bc or for broken string function in a
	(void)n;//stack+2, stack+3
__asm
        ; Algorithm is Duff`s device
	ldhl	sp,	#3
__endasm;
#ifdef __SDCC_BROKEN_STRING_FUNCTIONS
__asm
	ld	b, (hl)
	dec	hl
__endasm;
#else
__asm

	ld	a, (hl-)
	ld	b, a
	ld	a, c
__endasm;
#endif
__asm
	ld	c, (hl)
	ld	l, e
	ld	h, d
	;shift LSB to carry
	srl	b
	rr	c
	jr nc, skip_one
        ld	(hl+), a
skip_one:
	;n/2 in bc
	;shift second LSB to carry
	srl	b
	rr	c
        ;n/4 in bc
	inc	b
	inc	c
	jr nc, test
	jr	copy_two
copy_four:
        ld	(hl+), a
	ld	(hl+), a
copy_two:
        ld	(hl+), a
	ld	(hl+), a
test:
	dec	c
	jr	NZ, copy_four
	dec	b
	jr	NZ, copy_four
        ;restore dest
	ld	c, e
	ld	b, d
	pop	hl
	pop	af
	jp	(hl)
__endasm;
}
