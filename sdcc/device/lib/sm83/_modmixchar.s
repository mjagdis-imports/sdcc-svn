;--------------------------------------------------------------------------
;  __modmixchar.s
;
;  Copyright (C) 2000, Michael Hope
;  Copyright (C) 2021, Sebastian 'basxto' Riedel (sdcc@basxto.de)
;  Copyright (c) 2021, Philipp Klaus Krause
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

	.module __modmixchar
	.optsdcc -msm83 sdcccall(1)

        ;; Originally from GBDK by Pascal Felber.
.globl	__moduschar
.globl	__modsuchar

        .area   _CODE

__moduschar:
	ld	c, a
	ld	b, #0

	call	signexte

	ld	c, e
	ld	b, d

	ret



__modsuchar:
	ld	d, #0

	ld	c, a             ; Sign extend
	rlca
	sbc	a
	ld	b,a

	call	.divmods16

	ld	c, e
	ld	b, d

	ret
