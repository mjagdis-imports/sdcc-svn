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

void *memset (void *s, int c, size_t n) __naked
{
  (void)s;
  (void)c;
  (void)n;
  __asm
    pop   iy
    pop   bc
    push  hl
    ld    a, c
    or    a, b
    jr    Z, end
    ld    (hl), e
    dec   bc
    ld    a, c
    or    a, b
    jr    Z, end
    ld    e, l
    ld    d, h
    inc   de
    ldir 
end:
    pop   de
    jp	(iy)
  __endasm;
}
