/*-------------------------------------------------------------------------
   __mullong.c - routine for multiplication of 32 bit (unsigned) long

   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
   Copyright (C) 1999, Jean Louis VERN jlvern@writeme.com

   Modifications for PIC14 by
   Copyright (C) 2019 Gonzalo Pérez de Olaguer Córdoba <salo@gpoc.es>

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

/* Signed and unsigned multiplication are the same - as long as the output
   has the same precision as the input. */

#include <sdcc-lib.h>

#include <stdbit.h>

struct some_struct {
	short a ;
	char b;
	long c ;};

#if defined(__SDCC_hc08) || defined(__SDCC_s08) || defined(__SDCC_stm8)
/* big endian order */
union bil {
        struct {unsigned char b3,b2,b1,b0 ;} b;
        struct {unsigned short hi,lo ;} i;
        unsigned long l;
        struct { unsigned char b3; unsigned short i12; unsigned char b0;} bi;
} ;
#else
/* little endian order */
union bil {
        struct {unsigned char b0,b1,b2,b3 ;} b;
        struct {unsigned short lo,hi ;} i;
        unsigned long l;
        struct { unsigned char b0; unsigned short i12; unsigned char b3;} bi;
} ;
#endif

#define bcast(x) ((union bil _AUTOMEM *)&(x))

/*
                     3   2   1   0
       X             3   2   1   0
       ----------------------------
                   0.3 0.2 0.1 0.0
               1.3 1.2 1.1 1.0
           2.3 2.2 2.1 2.0
       3.3 3.2 3.1 3.0
       ----------------------------
                  |3.3|1.3|0.2|0.0|   A
                    |2.3|0.3|0.1|     B
                    |3.2|1.2|1.0|     C
                      |2.2|1.1|       D
                      |3.1|2.0|       E
                        |2.1|         F
                        |3.0|         G
                          |-------> only this side 32 x 32 -> 32
*/

#if defined(__SDCC_pic14)
long
__mullong (long a, long b)
{
        union bil x;
        union bil y;
        union bil t;
	x.l = a;
	y.l = b;

        t.i.hi = x.b.b0 * y.b.b2;           // A
        t.i.lo = x.b.b0 * y.b.b0;           // A
        t.b.b3 += x.b.b3 * y.b.b0;          // G
        t.b.b3 += x.b.b2 * y.b.b1;          // F
        t.i.hi += x.b.b2 * y.b.b0;          // E <- b lost in .lst
        // x.i.hi is free !
        t.i.hi += x.b.b1 * y.b.b1;          // D <- b lost in .lst

        x.bi.b3 = x.b.b1 * y.b.b2;  // C
        x.bi.i12 = x.b.b1 * y.b.b0; // C

        y.bi.b3 = x.b.b0 * y.b.b3;  // B
        y.bi.i12 = x.b.b0 * y.b.b1; // B

        y.bi.b0 = 0;                                // B
        x.bi.b0 = 0;                                // C
        t.l += (long)x.l;

        return t.l + (long)y.l;
}
#else
long
__mullong (long a, long b) __SDCC_NONBANKED
{
        union bil t;

        t.i.hi = bcast(a)->b.b0 * bcast(b)->b.b2;           // A
        t.i.lo = bcast(a)->b.b0 * bcast(b)->b.b0;           // A
        t.b.b3 += bcast(a)->b.b3 * bcast(b)->b.b0;          // G
        t.b.b3 += bcast(a)->b.b2 * bcast(b)->b.b1;          // F
        t.i.hi += bcast(a)->b.b2 * bcast(b)->b.b0;          // E <- b lost in .lst
        // bcast(a)->i.hi is free !
        t.i.hi += bcast(a)->b.b1 * bcast(b)->b.b1;          // D <- b lost in .lst

        bcast(a)->bi.b3 = bcast(a)->b.b1 * bcast(b)->b.b2;  // C
        bcast(a)->bi.i12 = bcast(a)->b.b1 * bcast(b)->b.b0; // C

        bcast(b)->bi.b3 = bcast(a)->b.b0 * bcast(b)->b.b3;  // B
        bcast(b)->bi.i12 = bcast(a)->b.b0 * bcast(b)->b.b1; // B

        bcast(b)->bi.b0 = 0;                                // B
        bcast(a)->bi.b0 = 0;                                // C
        t.l += a;

        return t.l + b;
}
#endif

