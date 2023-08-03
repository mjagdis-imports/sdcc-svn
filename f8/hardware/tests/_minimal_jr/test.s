.area HOME

	; A minimal test, to check that the basic test infrastructure works, using a relative jump.

loop:
	jr	#loop	; An endless loop, so we never fail until we reach the time limit.

