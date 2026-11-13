;--------------------------------------------------------------------------
;  __divuint.s
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

	.module __divuint
	.optsdcc -msm83 sdcccall(1)

        ;; Originally from GBDK by Pascal Felber.
.globl	__divuint

        .area   _CODE

__divuint:      
        ld	a, e
        ld	e, c
        ld	c, a

        ld	a, d
        ld	d, b
        ld	b, a

        jp	.divmodu16

__moduint:
        ld	a, e
        ld	e, c
        ld	c, a

        ld	a, d
        ld	d, b
        ld	b, a

        call	.divmodu16

	ld	c, e
	ld	b, d

        ret


.divmodu8::
        ld      b,#0x00
        ld      d,b
        ; Fall through to divu16

        ;; 16-bit division
        ;;
        ;; Entry conditions
        ;;   BC = dividend
        ;;   DE = divisor
        ;;
        ;; Exit conditions
        ;;   BC = quotient
        ;;   DE = remainder
        ;;   If divisor is non-zero, carry=0
        ;;   If divisor is 0, carry=1 and both quotient and remainder are 0
        ;;
        ;; Register used: AF,BC,DE,HL

.divmodu16::
        ;; Check for division by zero
        ld      a,e
        or      d
        jr      NZ,.divide      ; Branch if divisor is non-zero
        ld      bc,#0x00        ; Divide by zero error
        ld      d,b
        ld      e,c
        scf                     ; Set carry, invalid result
        ret
.divide:
        ld      l,c             ; L = low byte of dividend/quotient
        ld      h,b             ; H = high byte of dividend/quotient
        ld      bc,#0x00        ; BC = remainder
        or      a               ; Clear carry to start
        ld      a,#16           ; 16 bits in dividend
.dvloop:
        ;; Shift next bit of quotient into bit 0 of dividend
        ;; Shift next MSB of dividend into LSB of remainder
        ;; BC holds both dividend and quotient. While we shift a bit from
        ;;  MSB of dividend, we shift next bit of quotient in from carry
        ;; HL holds remainder
        ;; Do a 32-bit left shift, shifting carry to L, L to H,
        ;;  H to C, C to B
        push    af              ; save number of bits remaining
        rl      l               ; Carry (next bit of quotient) to bit 0
        rl      h               ; Shift remaining bytes
        rl      c
        rl      b               ; Clears carry since BC was 0
        ;; If remainder is >= divisor, next bit of quotient is 1. This
        ;;  bit goes to carry
        push    bc              ; Save current remainder
        ld      a,c             ; Subtract divisor from remainder
        sbc     e
        ld      c,a
        ld      a,b
        sbc     d
        ld      b,a
        ccf                     ; Complement borrow so 1 indicates a
                                ;  successful subtraction (this is the
                                ;  next bit of quotient)
        jr      C,.drop         ; Jump if remainder is >= dividend
        pop     bc              ; Otherwise, restore remainder
        pop     af              ; recover # bits remaining, carry flag destroyed
        dec     a
        or      a               ; restore (clear) the carry flag
        jr      NZ,.dvloop
        jr      .nodrop
.drop:
        pop     af              ; faster and smaller than 2x inc sp
        pop     af              ; recover # bits remaining, carry flag destroyed
        dec     a
        scf                     ; restore (set) the carry flag
        jr      NZ,.dvloop
        jr      .nodrop
.nodrop:
        ;; Shift last carry bit into quotient
        ld      d,b             ; DE = remainder
        ld      e,c
        rl      l               ; Carry to L
        ld      c,l             ; C = low byte of quotient
        rl      h
        ld      b,h             ; B = high byte of quotient
        or      a               ; Clear carry, valid result
        ret

