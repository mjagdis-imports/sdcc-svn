;-------------------------------------------------------------------------
;  _logexpf.s
;
;  Copyright (C) 2005, Paul Stoffregen
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
;-------------------------------------------------------------------------

	.module _logexpf
	.include "config.inc"

	.globl __fs_cordic_rshift_r765_unsigned
	.globl __fs_natural_log_table
	.globl _logexpf_tmp


	.area DSEG    (DATA)
_logexpf_tmp:
	.ds 4

	.area CSEG    (CODE)

__fs_cordic_rshift_r765_unsigned:
	add	a, #248
	jnc	00003$
	mov	b, r5
	mov	r5, ar6
	mov	r6, ar7
	mov	r7, #0
	add	a, #248
	jnc	00003$
	mov	b, r5
	mov	r5, ar6
	mov	r6, #0
	add	a, #248
	jnc	00003$
	mov	b, r5
	mov	r5, #0
	add	a, #248
	jnc	00003$
	mov	b, #0
	ret
00003$:
	add	a, #8
	jz	00030$
	push	ar0
	mov	r0, a
00010$:
	clr	c
	mov	a, r7
	rrc	a
	mov	r7, a
	mov	a, r6
	rrc	a
	mov	r6, a
	mov	a, r5
	rrc	a
	mov	r5, a
	mov	a, b
	rrc	a
	mov	b, a
	djnz	r0, 00010$
	pop	ar0
00030$:
	ret

	.area CONST   (CODE)

__fs_natural_log_table:
.db 0xFF, 0x42, 0x2E, 0x16         ; 0.693147180560
.db 0xF6, 0x91, 0xF9, 0x0C         ; 0.405465108108
.db 0xF2, 0xFD, 0x23, 0x07         ; 0.223143551314
.db 0xEE, 0xE0, 0xC4, 0x03         ; 0.117783035656
.db 0x0C, 0xA3, 0xF0, 0x01         ; 0.060624621816
.db 0xD8, 0x14, 0xFC, 0x00         ; 0.030771658667
.db 0xA3, 0x02, 0x7F, 0x00         ; 0.015504186536
.db 0x55, 0xC0, 0x3F, 0x00         ; 0.007782140442
.db 0x0B, 0xF0, 0x1F, 0x00         ; 0.003898640416
.db 0x01, 0xFC, 0x0F, 0x00         ; 0.001951220131
.db 0x00, 0xFF, 0x07, 0x00         ; 0.000976085973
.db 0xC0, 0xFF, 0x03, 0x00         ; 0.000488162080
.db 0xF0, 0xFF, 0x01, 0x00         ; 0.000244110828
.db 0xFC, 0xFF, 0x00, 0x00         ; 0.000122062863
.db 0xFF, 0x7F, 0x00, 0x00         ; 0.000061033294
.db 0x00, 0x40, 0x00, 0x00         ; 0.000030517112
.db 0x00, 0x20, 0x00, 0x00         ; 0.000015258673
.db 0x00, 0x10, 0x00, 0x00         ; 0.000007629365
.db 0x00, 0x08, 0x00, 0x00         ; 0.000003814690
.db 0x00, 0x04, 0x00, 0x00         ; 0.000001907347
.db 0x00, 0x02, 0x00, 0x00         ; 0.000000953674
.db 0x00, 0x01, 0x00, 0x00         ; 0.000000476837
.db 0x80, 0x00, 0x00, 0x00         ; 0.000000238419
.db 0x40, 0x00, 0x00, 0x00         ; 0.000000119209
.db 0x20, 0x00, 0x00, 0x00         ; 0.000000059605
.db 0x10, 0x00, 0x00, 0x00         ; 0.000000029802
.db 0x08, 0x00, 0x00, 0x00         ; 0.000000014901
.db 0x04, 0x00, 0x00, 0x00         ; 0.000000007451
.db 0x02, 0x00, 0x00, 0x00         ; 0.000000003725
.db 0x01, 0x00, 0x00, 0x00         ; 0.000000001863

