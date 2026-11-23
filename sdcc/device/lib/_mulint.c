/*-------------------------------------------------------------------------
   __mulint.c - routine for (unsigned) int (16 bit) multiplication

   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net

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

#include <sdcc-lib.h>

#include <stdbit.h>

#if __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_BIG__
union uu {
	struct { unsigned char hi, lo; } s;
        unsigned int t;
};
#else
union uu {
	struct { unsigned char lo, hi; } s;
        unsigned int t;
};
#endif

int
__mulint (int a, int b) __SDCC_NONBANKED
{
#if !defined(__SDCC_STACK_AUTO) && (defined(__SDCC_MODEL_LARGE) || defined(__SDCC_ds390))	// still needed for large
	union uu __xdata *x;
	union uu __xdata *y;
	union uu t;
        x = (union uu __xdata *)&a;
        y = (union uu __xdata *)&b;
#elif defined(__SDCC_pic14)
	union uu x;
	union uu y;
	union uu t;
        x.t = a;
        y.t = b;
#else
	register union uu *x;
	register union uu *y;
	union uu t;
        x = (union uu *)&a;
        y = (union uu *)&b;
#endif

  // sdcc is bad at handling union accesses. So we use (unsigned char)a instead of x->s.lo here.
#if defined(__SDCC_pic14)
  t.t = x.s.lo * y.s.lo;
  t.s.hi += (x.s.lo * y.s.hi) + (x.s.hi * y.s.lo);
#else
  t.t = (unsigned char)a * (unsigned char)b;
  t.s.hi += ((unsigned char)a * y->s.hi) + (x->s.hi * (unsigned char)b);
#endif

  return t.t;
}

