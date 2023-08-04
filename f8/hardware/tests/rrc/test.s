.area HOME

	; Test 8-bit rotate right trough carry

	ld	xl, #0xaa
	add	xl, #0 ; clear carry
	rrc	xl
	jrz	#l1trap
	jrc	#l1trap
	cp	xl, #0x55
	jrz	#l1
l1trap:
	trap
l1:

	add	xl, #0 ; clear carry
	rrc	xl
	jrz	#l2trap
	jrnc	#l2trap
	cp	xl, #0x2a
	jrz	#l2
l2trap:
	trap
l2:

	ld	xl, #0xff
	add	xl, #0xab ; set carry, set xl to 0xaa
	rrc	xl
	jrz	#l3trap
	jrc	#l3trap
	cp	xl, #0xd5
	jrz	#l3
l3trap:
	trap
l3:

	ld	xl, #0x01
	add	xl, #0 ; clear carry
	rrc	xl
	jrnz	#l4trap
	jrnc	#l4trap
	cp	xl, #0x00
	jrz	#l4
l4trap:
	trap
l4:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

