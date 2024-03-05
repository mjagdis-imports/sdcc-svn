IRQCTRLEN	.equ	0x0010
IRQCTRLACT	.equ	0x0012

TIMER0CTRL	.equ	0x0018
TIMER0CNT	.equ	0x001a
TIMER0RLD	.equ	0x001c
TIMER0CMP	.equ	0x001e

GPIO0DDR	.equ	0x0028
GPIO0ODR	.equ	0x002a
GPIO0IDR	.equ	0x002c
GPIO0PR	.equ	0x002e

WATCHDOGCTRL	.equ	0x0020

.area HOME(ABS)

	; Test timer peripheral.
.org	0x4000
	jp	#_main

.org	0x4004

int:
	push	#0
	xch	f, (0, sp)

	incw	z
	jrnz	int_end

	ldw	z, #64536
	inc	GPIO0ODR	; Increment LED counter once per second

int_end:
	clr	IRQCTRLACT	; clear interrupt request

	xch	f, (0, sp)
	addw	sp, #1
	reti

_main:
	; Set stack pointer one above RAM.
	ldw     y, #0x4000
	ldw     sp, y

	; Set up gpio
	clr	GPIO0ODR
	ld	xl, #0x1f
	ld	GPIO0DDR, xl

	; Check watchdog.
	ld	xl, WATCHDOGCTRL
	and	xl, #0x07
	ld	GPIO0ODR, xl
dogbite:
	jrnz	dogbite

	; Set up timer
	ldw	z, #64536
	ldw	y, #62536	; At 3 Mhz system clock, get a 1 KHz interrupt.
	ldw	TIMER0CNT, y
	ldw	TIMER0RLD, y
	inc	TIMER0CTRL ; Set to 1 (reset value is 0): start timer (at system clock).
	nop
	ld	xl, #0x01
	ld	IRQCTRLEN, xl ; Enable timer 0 overflow interrupt.

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

