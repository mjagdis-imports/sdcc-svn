.area HOME

	; Test 8-bit addition

	ld	xl, #0
	add	xl, #0
	jrc	#l1trap
	jrnz	#l1trap
	jrn	#l1trap
	cp	xl, #0
	jrz	#l1
l1trap:
	trap
l1:

	add	xl, #1
	jrc	#l2trap
	jrz	#l2trap
	jrn	#l2trap
	cp	xl, #1
	jrz	#l2
l2trap:
	trap
l2:

	add	xl, #0xff
	jrnc	#l3trap
	jrnz	#l3trap
	jrn	#l3trap
	cp	xl, #0
	jrz	#l3
l3trap:
	trap
l3:

	ldw	y, #0xff80
	add	xl, yl
	jrc	#l4trap
	jrz	#l4trap
	jrnn	#l4trap
	cp	xl, #0x80
	jrz	#l4
l4trap:
	trap
l4:

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

