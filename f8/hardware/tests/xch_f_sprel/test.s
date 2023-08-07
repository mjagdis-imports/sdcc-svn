.area HOME

	; Test xch f, (n, sp)
	
	; Set stack pointer one above RAM.
	ldw	y, #0x4000

	; Test the official flags first	
	ldw	sp, y
	tst	xl	; clear carry
	ld	xl, #0
	add	xl, #0	; c reset, n reset, z set.
	push	#0x00	; safe value to load into f
	xch	f, (0, sp)
	jrc	l1trap
	jrn	l1trap
	jrz	l1trap
	jro	l1trap
	ld	xl, (0, sp)
	and	xl, #0x0e	; mask out everything except for c, n, z.
	cp	xl, #0x08
	jrz	l1
l1trap:
	trap
l1:
	xch	f, (0, sp)
	jrc	l1trap
	jrn	l1trap
	jrnz	l1trap

	push	#0x1f	; All non-hidden flags set (once this goes into f)
	xch	f, (0, sp)
	jrnc	l2trap
	jrnn	l2trap
	jrnz	l2trap
	jro	l2
l2trap:
	trap
l2:

	; Test hidden flags, too.
	push	#0x00
	push	#0xff
	xch	f, (0, sp)
	xch	f, (1, sp)
	jrc	l3trap
	jrn	l3trap
	jrz	l3trap
	jro	l3trap
	ld	xl, (1, sp)
	cp	xl, #0xff
	jrz	l3
l3trap:
	trap
l3:

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

