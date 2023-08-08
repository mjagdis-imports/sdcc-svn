.area HOME

	; Test cltz.

	ldw	y, #0xffff
	cltz	y
	cpw	y, #0x0000
	jrz	l1
	trap
l1:
	cltz	y
	cpw	y, #0x1010
	jrz	l2
	trap
l2:

	ldw	y, #0x00ff
	cltz	y
	cpw	y, #0x0008
	jrz	l3
	trap
l3:
	ldw	y, #0xff00
	cltz	y
	cpw	y, #0x0800
	jrz	l4
	trap
l4:

	ldw	x, #0x0ff0
	cltz	x
	cpw	x, #0x0404
	jrz	l5
	trap
l5:
	ldw	z, #0x0770
	cltz	z
	cpw	z, #0x0405
	jrz	l6
	trap
l6:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

