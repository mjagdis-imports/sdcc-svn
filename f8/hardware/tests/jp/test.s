.area HOME

	; Test jp.

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

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

