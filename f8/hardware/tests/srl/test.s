.area HOME

	; Test logic right shift

	ld	xl, #0xaa
	srl	xl
	jrz	#l1trap
	jrc	#l1trap
	cp	xl, #0x55
	jrz	#l1
l1trap:
	trap
l1:

	srl	xl
	jrz	#l2trap
	jrnc	#l2trap
	cp	xl, #0x2a
	jrz	#l2
l2trap:
	trap
l2:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

