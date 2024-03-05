GPIO0DDR	.equ	0x0028
GPIO0ODR	.equ	0x002a
GPIO0IDR	.equ	0x002c
GPIO0PR	.equ	0x002e

.area HOME

	; Test gpio peripheral
	ld	xl, #0x55
	ld	GPIO0ODR, xl
	cp	xl, GPIO0ODR
	jrnz	l1trap
	ld	xl, #0xa5
	ld	GPIO0PR, xl
	cp	xl, GPIO0PR
	jrnz	l1trap
	ld	xl, #0x7f
	ld	GPIO0DDR, xl
	cp	xl, GPIO0DDR
	jrnz	l1trap
	nop
	nop
	nop
	nop
	ld	xl, GPIO0IDR
	and	xl, #0x7f
	cp	xl, #0x55
	jrz	l1
l1trap:
	trap
l1:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

