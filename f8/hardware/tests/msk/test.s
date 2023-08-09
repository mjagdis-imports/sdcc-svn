.area HOME

	; Test msk.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	push	#0xff
	ldw	y, #0x3fff
	ld	xl, #0x00
	msk	(y), xl, #0x55
	ld	xl, (0, sp)
	cp	xl, #0xaa
	jrnz	#l1trap
	ld	xl, #0x05
	msk	(y), xl, #0x0f
	ld	xl, (0, sp)
	cp	xl, #0xa5
	jrnz	#l1trap
	
	jp	#l1
l1trap:
	trap
l1:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

