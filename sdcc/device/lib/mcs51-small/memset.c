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
//void *memset (void *s, unsigned char c, size_t n)
#else
//void *memset (void *s, int c, size_t n)
#endif

  /* assembler implementation for mcs51 */
  static void dummy(void) __naked
  {
    __asm

    #if defined(__SDCC_NOOVERLAY)
        .area DSEG    (DATA)
    #else
        .area OSEG    (OVR,DATA)
    #endif

        _memset_PARM_2::
              .ds 1
        _memset_PARM_3::
              .ds 2

        .area CSEG    (CODE)

        _memset::

        ;   Assign buf (b holds memspace, no need to touch)
                mov     r4,dpl
                mov     r5,dph
                ;
        ;   Assign count
                mov     r6,_memset_PARM_3
                mov     r7,(_memset_PARM_3 + 1)
                ;
        ;   if (!count) return buf;
        ;   check for count != 0 intermangled with gymnastic
        ;   preparing djnz instructions
                cjne    r6,#0x00,COUNT_LSB_NOT_ZERO
                mov     a,r7
                jz      MEMSET_END
                dec     r7
        COUNT_LSB_NOT_ZERO:
                inc     r7
                ;
        ;   Assign ch
                mov     a,_memset_PARM_2

        /* now independent of the parameter passing everything
           should be in registers by now and the loop may start */
        ;   _memset.c do {

        MEMSET_LOOP:
        ;   _memset.c *p = ch;
                lcall   __gptrput

        ;   _memset.c p++;
                inc     dptr

        ;   _memset.c } while(--count) ;
                djnz    r6,MEMSET_LOOP
                djnz    r7,MEMSET_LOOP
                ;

        MEMSET_END:
        ;   _memset.c return buf ;
                ; b was unchanged
                mov     dpl,r4
                mov     dph,r5
                ;
                ret

    __endasm;
  }

