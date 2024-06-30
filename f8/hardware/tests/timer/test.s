IRQCTRLEN	.equ	0x0010
IRQCTRLACT	.equ	0x0012

TIMER0CTRL	.equ	0x0018
TIMER0CNT	.equ	0x001a
TIMER0RLD	.equ	0x001c
TIMER0CMP	.equ	0x001e


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

	; Test read/write of 16-bit timer registers.
	ldw	y, #0xa5b6
	ldw	TIMER0CNT, y
	ldw	y, #0x5a6b
	ldw	TIMER0RLD, y
	ldw	y, TIMER0CNT
	cpw	y, #0xa5b6
	jrnz	l1trap
	ldw	y, TIMER0RLD
	cpw	y, #0x5a6b
	jrz	l1
l1trap:
	trap
l1:

	; Do not change, add or remove instructions from here (we want to eventually trigger an interrupt during ret)!
	ld	xl, #1
	ld	0x3ff0, xl
	ldw	y, #0xffea
	ldw	TIMER0CNT, y
	ldw	TIMER0RLD, y
	inc	TIMER0CTRL ; Set to 1 (reset value is 0): start timer (at system clock).
	nop
	ld	xl, #0x01
	ld	IRQCTRLEN, xl ; Enable timer 0 overflow interrupt.
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	jp	#loop
	trap

func:
	nop
	nop
	ret
	trap
	; Do not change, add or remove instructions before here!

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.


