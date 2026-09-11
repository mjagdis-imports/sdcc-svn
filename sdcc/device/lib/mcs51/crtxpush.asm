;--------------------------------------------------------------------------
;  crtxpush.asm :- C run-time: push registers (not R0) to xstack
;
;  Copyright (C) 2009, Maarten Brock
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
;  might be covered by the GNU General Public License.
;--------------------------------------------------------------------------

;--------------------------------------------------------
; overlayable bit register bank
;--------------------------------------------------------
	.area BIT_BANK	(REL,OVR,DATA)
bits:
	.ds 1

	.area HOME    (CODE)

; Push registers r0..r7, dpl, dph & bits on xstack
; Expect allocation size in ACC[3-0] and mask in ACC[7-4] & B
sdcc_xpush_regs::
	push	acc		;save mask in high nibble of acc
	anl	a,#0x0F		;take size from low nibble of acc
	add	a,_spx
	mov	_spx,a		;allocate space
	xch	a,r0
	push	acc		;save R0 for return
	jbc	B.0,00100$	;if B(0)=0 then
;	mov	a,r0		;acc already holds R0
	dec	r0
	movx	@r0,a		;push R0
00100$:
	jbc	B.1,00101$	;if B(1)=0 then
	mov	a,r1
	dec	r0
	movx	@r0,a		;push R1
00101$:
	jbc	B.2,00102$	;if B(2)=0 then
	mov	a,r2
	dec	r0
	movx	@r0,a		;push R2
00102$:
	jbc	B.3,00103$	;if B(3)=0 then
	mov	a,r3
	dec	r0
	movx	@r0,a		;push R3
00103$:
	jbc	B.4,00104$	;if B(4)=0 then
	mov	a,r4
	dec	r0
	movx	@r0,a		;push R4
00104$:
	jbc	B.5,00105$	;if B(5)=0 then
	mov	a,r5
	dec	r0
	movx	@r0,a		;push R5
00105$:
	jbc	B.6,00106$	;if B(6)=0 then
	mov	a,r6
	dec	r0
	movx	@r0,a		;push R6
00106$:
	jbc	B.7,00107$	;if B(7)=0 then
	mov	a,r7
	dec	r0
	movx	@r0,a		;push R7
00107$:
	pop	acc
	pop	B		;retrieve mask from high nibble of acc into B
	push	acc
	jbc	B.4,00108$	;if B(4)=0 then
	mov	a,bits
	dec	r0
	movx	@r0,a		;push bits
00108$:
	jbc	B.5,00109$	;if B(5)=0 then
	mov	a,dpl
	dec	r0
	movx	@r0,a		;push dpl
00109$:
	jbc	B.6,00110$	;if B(6)=0 then
	mov	a,dph
	dec	r0
	movx	@r0,a		;push dph
00110$:
	pop	acc
	mov	r0,a
	ret
