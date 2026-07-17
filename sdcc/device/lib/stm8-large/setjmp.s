;--------------------------------------------------------------------------
;  setjmp.s
;
;  Copyright (c) 2014-2026, Philipp Klaus Krause
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

	.area   CODE

	.globl ___setjmp

; int setjmp(jmp_buf env);
___setjmp:
	; store return address
	ldw	y, (1, sp)
	ldw	(x), y
	ld	a, (3, sp)
	ld	(2, x), a

	; store stack pointer
	ldw	y, sp
	ldw	(3, x), y

	; return 0
	clrw	x

	retf

	.globl _longjmp

; void longjmp(jmp_buf env, int val);
_longjmp:
	; Restore stack pointer while saving val in jmp_buf.
	ldw	y, x
	ldw	x, (3, x)
	ld	a, (4, sp)
	ld	(3, y), a
	ld	a, (5, sp)
	ld	(4, y), a
	ldw	sp, x
	popw	x
	pop	a

	; Calculate return value
	ldw	x, y
	ldw	x, (3, x)
	jrne	jump
	incw	x
jump:
	; return
	ld	a, (y)
	ldw	y, (1, y)
	pushw	y
	push	a
	retf

