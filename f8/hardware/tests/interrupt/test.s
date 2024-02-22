IRQCTRLEN	.equ	0x0010
IRQCTRLACT	.equ	0x0012

TIMER0CTRL	.equ	0x0018
TIMER0CNT	.equ	0x001a
TIMER0RLD	.equ	0x001c
TIMER0CMP	.equ	0x001e


.area HOME(ABS)

	; Test interrupt handling.
.org	0x4000
	jp	#_main

.org	0x4004

; For the first interrupt, don't clear it, to check that triggering again immediately works fine.
int:
	push	#0
	xch	f, (0, sp)
	dec	0x3ff0
	jrz	#intend
	clr	IRQCTRLACT
intend:
	xch	f, (0, sp)
	addw	sp, #1
	reti

_main:
	; Set stack pointer one above RAM.
	ldw     y, #0x4000
	ldw     sp, y

	ld	xl, #1
	ld	0x3ff0, xl
	ldw	y, #0xffea
	ldw	TIMER0CNT, y
	ldw	TIMER0RLD, y
	inc	TIMER0CTRL ; Set to 1 (reset value is 0): start timer (at system clock).
	nop
	ld	xl, #0x01
	ld	IRQCTRLEN, xl ; Enable timer 0 overflow interrupt.
	
	; Test some prefixed instructions (potentially using hidden flags) to ensure that interrupts don't break these.
	clrw	x
	clrw	y
	clrw	z
	tstw	x
	jrnz	l1trap
	tstw	y
	jrnz	l1trap
	tstw	z
	jrnz	l1trap
	ld	xl, #0x11
	ld	xh, #0x22
	ld	yl, #0x33
	ld	zl, #0x44
	cpw	x, #0x2211
	jrnz	l1trap
	cpw	y, #0x0033
	jrnz	l1trap
	cpw	z, #0x0044
	jrz	l1
l1trap:
	trap
l1:

	; Ensure that last instruction before an interrupted ret is not repeated.
	call	#f
	call	#f
	cp	xl, #0x13
	jrnz	l2trap
	call	#f
	call	#f
	cp	xl, #0x15
	jrnz	l2trap
	call	#f
	call	#f
	cp	xl, #0x17
	jrnz	l2trap
	call	#f
	call	#f
	cp	xl, #0x19
	jrnz	l2trap
	call	#f
	cp	xl, #0x1a
	jrz	l2
l2trap:
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

f:
	inc	xl
	ret


