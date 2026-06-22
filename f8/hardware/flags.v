/*-------------------------------------------------------------------------
  flags.v - f8 flag register

  Copyright (c) 2026, Philipp Klaus Krause philipp@colecovision.eu)

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2, or (at your option) any
  later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
-------------------------------------------------------------------------*/

`begin_keywords "1800-2009"

// Flag bit order chosen to facilitate efficient constant-time comparisons
// (ie. comparisons where the execution time does not depend on the compared values -
// which helps prevent timing side-channels).
// For this, we want the n and o flags next to each other, the c and z flags next to each other,
// the z flag next to to the n or o flag. Having the o flag as least significant bit
// facilitate efficient constant-time C23 checked integer arithmetic.
// Combined, these conditions fix the order.
typedef enum logic [2:0]
{
	FLAG_O,	// Overflow
	FLAG_N,	// Negative
	FLAG_Z,	// Zero
	FLAG_C,	// Carry
	FLAG_H,	// Half-carry
	FLAG_5,
	FLAG_6,
	FLAG_7
} flagname_t;

// Old flag bit order (up to SDCC 4.6.0)
// typedef enum logic [2:0]
// {
//	FLAG_H,	// Half-carry
//	FLAG_C,	// Carry
//	FLAG_N,	// Negative
//	FLAG_Z,	// Zero
//	FLAG_O,	// Overflow
//	FLAG_5,
//	FLAG_6,
//	FLAG_7
//} flagname_t;

`end_keywords

