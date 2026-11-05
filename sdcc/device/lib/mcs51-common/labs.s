;--------------------------------------------------------------------------
;  labs.s - computes absolute value of a long.
;
;  Copyright (C) 2004, Maarten Brock, sourceforge.brock@dse.nl
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

	.module labs
	.include "config.inc"

	.globl	_labs

	.area CSEG    (CODE)

_labs:
	jnb	acc.7, skip_neg
	mov	r2,a
	clr	c
	clr	a
	subb	a,dpl
	mov	dpl,a
	clr	a
	subb	a,dph
	mov	dph,a
	clr	a
	subb	a,b
	mov	b,a
	clr	a
	subb	a,r2
skip_neg:
	RET

