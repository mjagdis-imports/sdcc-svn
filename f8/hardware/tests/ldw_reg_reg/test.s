.area HOME

	; Test 16-bit register-from-register loads.

	ldw	y, #0xaa55
	ldw	x, #0xa5a5
	ldw	z, #0x5a5a
	ldw	x, y
	cpw	y, #0xaa55
	jrnz	l1trap
	cpw	x, #0xaa55
	jrnz	l1trap
	cpw	z, #0x5a5a
	jrnz	l1trap
	ldw	z, y
	cpw	z, #0xaa55
	jrnz	l1trap
	ldw	x, #0xa55a
	ldw	y, x
	cpw	y, #0xa55a
	jrz	l1
l1trap:
	trap
l1:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

