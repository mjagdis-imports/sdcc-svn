/*-------------------------------------------------------------------------
   _mullonglong.c - routine for multiplication of 64 bit long long

   Copyright (C) 2012, Philipp Klaus Krause . philipp@informatik.uni-frankfurt.de

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

#include <stdbit.h>
#include <stdint.h>

#include <sdcc-lib.h>

#ifdef __SDCC_LONGLONG

#if __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_LITTLE__
# define LSB_OFFSET     (0)
# define MSB_DIRECTION  (+1)
#elif __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_BIG__
# define LSB_OFFSET     (sizeof(long long) - 1)
# define MSB_DIRECTION  (-1)
#endif

/* On most targets & models it costs less bytes, less spill locations and
   sometimes even less cycles to do it per bit as below.
   The hc08/s08 are the clear exception and do better with byte multiplication.
*/

#if defined(__SDCC_s08) || defined(__SDCC_hc08)

long long _mullonglong(long long ll, long long lr) __SDCC_NONBANKED
{
  unsigned long long ret = 0ull;
  unsigned char i, j, k;

  unsigned char _AUTOMEM * pl = ((unsigned char _AUTOMEM *)&ll) + LSB_OFFSET;
  for (i = 0; i < sizeof (long long); i++)
    {
      unsigned char l = *pl;
      pl += MSB_DIRECTION;
      if (l)
        {
          unsigned char _AUTOMEM * pr = (unsigned char _AUTOMEM *)&lr + LSB_OFFSET;
          for (j = 0; (unsigned char)(i + j) < sizeof (long long); j++)
            {
              unsigned char r = *pr;
              pr += MSB_DIRECTION;
              if (r)
                {
                  unsigned long long mul = (unsigned short)(l * r);
                  k = i + j;
                  while (k--)
                    mul <<= 8;
                  ret += mul;
                }
            }
        }
    }

  return(ret);
}

#else

long long _mullonglong(long long x, long long y) __SDCC_NONBANKED
{
  long long result = 0ull;
  unsigned char i = sizeof(long long) * 8;

  do
    {
      result <<= 1;
      if (x & 0x8000000000000000)
        result += y;
      x <<= 1;
    }
  while (--i);

  return result;
}

#endif

#endif

