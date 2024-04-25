.area HOME

	; Test jp.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	jp	#l1
	trap

l2:
	ldw	y, #l3
	jp	y
	trap

l1:
	jp	#l2
	trap

l3:

	push	#0x00
	xch	f, (0, sp)
	jp	#l4
l4:
	xch	f, (0, sp)
	pop	xl
	cp	xl, #0x00
	jrnz	l6trap
	push	#0xff
	xch	f, (0, sp)
	jp	#l5
l5:
	xch	f, (0, sp)
	pop	xl
	cp	xl, #0xff
	jrz	l6
l6trap:
	trap
l6:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

