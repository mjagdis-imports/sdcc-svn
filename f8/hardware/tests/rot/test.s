.area HOME

	; Test 8-bit rotation.

	ld	xl, #0x55
	rot	xl, #1
	cp	xl, #0xaa
	jrnz	#l1trap
	rot	xl, #1
	cp	xl, #0x55
	jrnz	#l1trap
	rot	xl, #5
	cp	xl, #0xaa
	jrnz	#l1trap
	rot	xl, #5
	cp	xl, #0x55
	jrnz	#l1trap
	ld	xl, #0x0f
	rot	xl, #4
	cp	xl, #0xf0
	jrz	#l1
l1trap:
	trap
l1:

	ld	xh, #0x1f
	rot	xh, #1
	cp	xh, #0x3e
	jrnz	#l2trap
	ld	yl, #0xf1
	rot	yl, #1
	cp	yl, #0xe3
	jrnz	#l2trap
	ld	zl, #0x0f
	rot	zl, #2
	cp	zl, #0x3c
	jrnz	#l2trap
	rot	zl, #8
	cp	zl, #0x3c
	jrz	#l2
l2trap:
	trap
l2:

	ld	yh, #0x05
	rot	yh, #4
	jrz	l3trap
	jrn	l3trap
	cp	yh, #0x50
	jrnz	l3trap
	ld	zh, #0x01
	rot	zh, #7
	jrz	l3trap
	jrnn	l3trap
	cp	zh, #0x80
	jrz	l3
l3trap:
	trap
l3:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

