.area HOME

	; Test relative jumps.

	; Set stack pointer one above RAM.
	ldw	y, #0x4000
	ldw	sp, y

	jr	l1
	trap
l1:

	; all flags reset.
	push #0x00
	xch	f, (0,sp)
	jrc	l2trap
	jrz	l2trap
	jrn	l2trap
	jro	l2trap
	jrslt	l2trap
	jrsle	l2trap
	jrgt	l2trap
	jrnc	l2
l2trap:
	trap
l2:
	jrnz	l2_
	trap
l2_:

	; h flag set.
	push #0x01
	xch	f, (0,sp)
	jrc	l3trap
	jrz	l3trap
	jrn	l3trap
	jro	l3trap
	jrslt	l3trap
	jrsle	l3trap
	jrgt	l3trap
	jrnc	l3
l3trap:
	trap
l3:
	jrnn	l3_
	trap
l3_:

	; h and o flags set.
	push #0x11
	xch	f, (0,sp)
	jrc	l4trap
	jrz	l4trap
	jrn	l4trap
	jrno	l4trap
	jrsge	l4trap
	jrsgt	l4trap
	jrgt	l4trap
	jrnc	l4
l4trap:
	trap
l4:
	jro	l4_
	trap
l4_:

	; c flag set.
	push #0x02
	xch	f, (0,sp)
	jrnc	l5trap
	jrz	l5trap
	jrn	l5trap
	jro	l5trap
	jrslt	l5trap
	jrsle	l5trap
	jrle	l5trap
	jrc	l5
l5trap:
	trap
l5:
	jrsge	l5_
	trap
l5_:

	; z and n flags set.
	push #0x0c
	xch	f, (0,sp)
	jrc	l6trap
	jrnz	l6trap
	jrnn	l6trap
	jro	l6trap
	jrsge	l6trap
	jrsgt	l6trap
	jrgt	l6trap
	jrnc	l6
l6trap:
	trap
l6:
	jrsle	l6_
	trap
l6_:


loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

