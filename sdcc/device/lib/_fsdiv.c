/*-------------------------------------------------------------------------
   _fsdiv.c - Floating point library in optimized assembly for 8051

   Copyright (c) 2004, Paul Stoffregen, paul@pjrc.com

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


#define __SDCC_FLOAT_LIB
#include <float.h>
#include <sdcc-lib.h>

#ifdef FLOAT_ASM_MCS51

// float __fsdiv (float a, float b) __reentrant
static void dummy(void) __naked
{
	__asm
	.globl	___fsdiv
___fsdiv:
	// extract the two inputs, placing them into:
	//      sign     exponent   mantissa
	//      ----     --------   --------
	//  a:  sign_a   exp_a      r4/r3/r2
	//  b:  sign_b   exp_b      r7/r6/r5

	lcall	fsgetargs

	// compute final sign bit
	jnb	sign_b, 00001$
	cpl	sign_a
00001$:

	// if divisor is zero, ...
	cjne	r7, #0, 00003$
	// if dividend is also zero, return NaN
	cjne	r4, #0, 00002$
	ljmp	fs_return_nan
00002$:
	// but dividend is non-zero, return infinity
	ljmp	fs_return_inf
00003$:
	// if dividend is zero, return zero
	cjne	r4, #0, 00004$
	ljmp	fs_return_zero
00004$:
	// if divisor is infinity, ...
	mov	a, exp_b
	cjne	a, #0xFF, 00006$
	// and dividend is also infinity, return NaN
	mov	a, exp_a
	cjne	a, #0xFF, 00005$
	ljmp	fs_return_nan
00005$:
	// but dividend is not infinity, return zero
	ljmp	fs_return_zero
00006$:
	// if dividend is infinity, return infinity
	mov	a, exp_a
	cjne	a, #0xFF, 00007$
	ljmp	fs_return_inf
00007$:

	// subtract exponents
	clr	c
	subb	a, exp_b
	// if no carry then no underflow
	jnc	00008$
	add	a, #127
	jc	00009$
	ljmp	fs_return_zero

00008$:
	add	a, #128
	dec	a
	jnc	00009$
	ljmp	fs_return_inf

00009$:
	mov	exp_a, a

	// need extra bits on a's mantissa
#ifdef FLOAT_FULL_ACCURACY
	clr	c
	mov	a, r5
	subb	a, r2
	mov	a, r6
	subb	a, r3
	mov	a, r7
	subb	a, r4
	jc	00010$
	dec	exp_a
	clr	c
	mov	a, r2
	rlc	a
	mov	r1, a
	mov	a, r3
	rlc	a
	mov	r2, a
	mov	a, r4
	rlc	a
	mov	r3, a
	clr	a
	rlc	a
	mov	r4, a
	sjmp	00011$
00010$:
#endif
	clr	a
	xch	a, r4
	xch	a, r3
	xch	a, r2
	mov	r1, a
00011$:

	// begin long division
	push	exp_a
#ifdef FLOAT_FULL_ACCURACY
	mov	b, #25
#else
	mov	b, #24
#endif
00012$:
	// compare
	clr	c
	mov	a, r1
	subb	a, r5
	mov	a, r2
	subb	a, r6
	mov	a, r3
	subb	a, r7
	mov	a, r4
	subb	a, #0		// carry==0 if mant1 >= mant2

#ifdef FLOAT_FULL_ACCURACY
	djnz	b, 00013$
	sjmp	00015$
00013$:
#endif
	jc	00014$
	// subtract
	mov	a, r1
	subb	a, r5
	mov	r1, a
	mov	a, r2
	subb	a, r6
	mov	r2, a
	mov	a, r3
	subb	a, r7
	mov	r3, a
	mov	a, r4
	subb	a, #0
	mov	r4, a
	clr	c

00014$:
	// shift result
	cpl	c
	mov	a, r0
	rlc	a
	mov	r0, a
	mov	a, dpl
	rlc	a
	mov	dpl, a
	mov	a, dph
	rlc	a
	mov	dph, a

	// shift partial remainder
	clr	c
	mov	a, r1
	rlc	a
	mov	r1, a
	mov	a, r2
	rlc	a
	mov	r2, a
	mov	a, r3
	rlc	a
	mov	r3, a
	mov	a, r4
	rlc	a
	mov	r4, a

#ifdef FLOAT_FULL_ACCURACY
	sjmp	00012$
00015$:
#else
	djnz	b, 00012$
#endif

	// now we've got a division result, so all we need to do
	// is round off properly, normalize and output a float

#ifdef FLOAT_FULL_ACCURACY
	cpl	c
	clr	a
	mov	r1, a
	addc	a, r0
	mov	r2, a
	clr	a
	addc	a, dpl
	mov	r3, a
	clr	a
	addc	a, dph
	mov	r4, a
	pop	exp_a
	jnc	00016$
	inc	exp_a
	// incrementing exp_a without checking carry is dangerous
	mov	r4, #0x80
00016$:
#else
	mov	r1, #0
	mov	a, r0
	mov	r2, a
	mov	r3, dpl
	mov	r4, dph
	pop	exp_a
#endif

	lcall	fs_normalize_a
	ljmp	fs_zerocheck_return
	__endasm;
}

#else

/*
** libgcc support for software floating point.
** Copyright (C) 1991 by Pipeline Associates, Inc.  All rights reserved.
** Permission is granted to do *anything* you want with this file,
** commercial or otherwise, provided this message remains intact.  So there!
** I would appreciate receiving any updates/patches/changes that anyone
** makes, and am willing to be the repository for said changes (am I
** making a big mistake?).
**
** Pat Wood
** Pipeline Associates, Inc.
** pipeline!phw@motown.com or
** sun!pipeline!phw or
** uunet!motown!pipeline!phw
*/

/* (c)2000/2001: hacked a little by johan.knol@iduna.nl for sdcc */

union float_long
  {
    float f;
    long l;
  };

/* divide two floats */
static float __fsdiv_org (float a1, float a2) __SDCC_FLOAT_NONBANKED
{
  volatile union float_long fl1, fl2;
  long result;
  unsigned long mask;
  unsigned long mant1, mant2;
  int exp;
  char sign;

  fl1.f = a1;

  exp = EXP (fl1.l);
  /* numerator denormal??? */
  if (!exp)
    return (0);

  fl2.f = a2;
  /* subtract exponents */
  exp -= EXP (fl2.l);
  exp += EXCESS;

  /* compute sign */
  sign = SIGN (fl1.l) ^ SIGN (fl2.l);

  /* now get mantissas */
  mant1 = MANT (fl1.l);
  mant2 = MANT (fl2.l);

  /* this assures we have 24 bits of precision in the end */
  if (mant1 < mant2)
    {
      mask = 0x1000000;
    }
  else
    {
      mask = 0x0800000;
      exp++;
    }

  if (exp < 1) /* denormal */
    return (0);

  if (exp >= 255)
    {
      fl1.l = sign ? SIGNBIT | __INFINITY : __INFINITY;
    }
  else
    {
      /* now we perform repeated subtraction of fl2.l from fl1.l */
      result = 0;
      do
        {
          long diff = mant1 - mant2;
          if (diff >= 0)
            {
              mant1 = diff;
              result |= mask;
            }
          mant1 <<= 1;
          mask >>= 1;
        }
      while (mask);

      /* round */
      if (mant1 >= mant2)
        result += 1;

      result &= ~HIDDEN;

      /* pack up and go home */
      fl1.l = PACK (sign ? SIGNBIT : 0 , exp, result);
    }
  return (fl1.f);
}

float __fsdiv (float a1, float a2) __SDCC_FLOAT_NONBANKED
{
  unsigned long _AUTOMEM *p2 = (unsigned long *) &a2;

  if (EXP (*p2) == 0) // a2 is denormal or zero, treat as zero
    {
      float f;
      unsigned long _AUTOMEM *p = (unsigned long *) &f;
      if (a1 > 0.0f)
        *p = __INFINITY;           // +inf
      else if (a1 < 0.0f)
        *p = SIGNBIT | __INFINITY; // -inf
      else // a1 is denormal, zero or nan
        *p = __NAN;                // nan
      return f;
    }
  return __fsdiv_org (a1, a2);
}

#endif
