IRQCTRLEN	.equ	0x0010
IRQCTRLACT	.equ	0x0012

WATCHDOGCTRL	.equ	0x0020
WATCHDOGCNT	.equ	0x0022
WATCHDOGRLD	.equ	0x0024


.area HOME(ABS)

	; Test timer peripheral.
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

	clrw	WATCHDOGRLD
	ldw	y, #0xfff0
	ldw	WATCHDOGCNT, y
	ld	xl, #0x01
	ld	WATCHDOGCTRL, xl

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.


