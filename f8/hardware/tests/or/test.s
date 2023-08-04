.area HOME

	; Test 8-bit or.

	ld	xl, #0
	or	xl, #0
	jrnz	#l1trap
	jrn	#l1trap
	cp	xl, #0
	jrz	#l1
l1trap:
	trap
l1:

	or	xl, #0x55
	jrz	#l2trap
	jrn	#l2trap
	cp	xl, #0x55
	jrz	#l2
l2trap:
	trap
l2:

	or	xl, #0xaa
	jrz	#l3trap
	jrnn	#l3trap
	cp	xl, #0xff
	jrz	#l3
l3trap:
	trap
l3:

	ld	xl, #0xaa
	ldw	y, #0x55
	or	xl, yl
	jrz	#l4trap
	jrnn	#l4trap
	cp	xl, #0xff
	jrz	#l4
l4trap:
	trap
l4:

	or	xl, #0x13
	jrz	#l5trap
	jrnn	#l5trap
	cp	xl, #0xff
	jrz	#l5
l5trap:
	trap
l5:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

