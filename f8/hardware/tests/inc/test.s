.area HOME

	; Test 8-bit increment.

	ld	xl, #0
	inc	xl
	jrc	#l1trap
	jrz	#l1trap
	jrn	#l1trap
	cp	xl, #1
	jrz	#l1
l1trap:
	trap
l1:

	ldw	y, #0
	inc	yl
	jrc	#l2trap
	jrz	#l2trap
	jrn	#l2trap
	cpw	y, #1
	jrz	#l2
l2trap:
	trap
l2:

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y
	ld	xl, #0
	push	xl
	inc	xl
	inc	(0, sp)
	jrc	#l3trap
	jrz	#l3trap
	jrn	#l3trap
	cp	xl, (0, sp)
	jrz	#l3
l3trap:
	trap
l3:

	ld	xl, #0xff
	inc	xl
	jrnc	#l4trap
	jrnz	#l4trap
	jrn	#l4trap
	cp	xl, #0x00
	jrz	#l4
l4trap:
	trap
l4:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

