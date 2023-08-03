.area HOME

	; A minimal test, to check that the basic test infrastructure works, using an absolute jump.

loop:
	jp	#loop	; An endless loop, so we never fail until we reach the time limit.

