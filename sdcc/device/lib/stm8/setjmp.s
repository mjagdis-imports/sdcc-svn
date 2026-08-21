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
	; store stack pointer
	ldw	y, sp
	ldw	(2, x), y

	; store return address
	popw	y
	ldw	(x), y

	; return 0
	clrw	x

	jp	(y)

	.globl _longjmp
	
; void longjmp(jmp_buf env, int val);
_longjmp:
	; Restore stack pointer while saving val in jmp_buf.
	ldw	y, x
	ldw	x, (2, x)
	ld	a, (3, sp)
	ld	(2, y), a
	ld	a, (4, sp)
	ld	(3, y), a
	ldw	sp, x
	popw	x

	; Calculate return value
	ldw	x, y
	ldw	x, (2, x)
	jrne	jump
	incw	x
jump:
	; Return
	ldw	y, (y)
	jp	(y)

