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

.area HOME(ABS)

	; Test timer peripheral.
.org	0x4000
	jp	#_main

.org	0x4004

; For the first interrupt, don't clear it, to check that triggering again immediately works fine.
int:
	push	#0
	pushw	y

	ldw	y, 0x3f80
	incw	y
	ldw	0x3f80, y

	tstw	y
	jrnz	intend
	ldw	y, #0xfffe
	ldw	0xff80, y
	ld	xl, GPIO0ODR
	inc	xl
	ld	GPIO0ODR, xl

intend:
	clr	IRQCTRLACT
	popw	y
	xch	f, (0, sp)
	addw	sp, #1
	reti

_main:
	; Set stack pointer one above RAM.
	ldw     y, #0x4000
	ldw     sp, y

	ldw	y, #0xfffe
	ldw	0x3f80, y

	clr	GPIO0ODR
	ld	xl, #0x1f
	ld	GPIO0DDR, xl

	ld	xl, #1
	ld	0x3ff0, xl
	ldw	y, #0xffd0
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
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	call	#func
	ld	xl, GPIO0ODR
	cp	xl, #2
	jrc	loop
	trap

func:
	nop
	nop
	ret
	trap

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.


