; assembler configuration
.optsdcc -mmcs51 --model-medium

; defines for conditional code
.define MCS51
.define MODEL_MEDIUM

; pseudo instructions
.define RET "ret"

; float defines

; This adds extra code for proper round-off, in
; an attempt to match the results from gcc.
.define FLOAT_FULL_ACCURACY

; This adds about 66 bytes to the code size and
; significantly speeds up shift operations more
; than 8 bits (common when subtracting numbers
; of significantly different magnitude and scaling
; to fixed point)
.define FLOAT_SHIFT_SPEEDUP

.define sign_a  "psw.1"
.define sign_b  "psw.5"
.define exp_a "dpl"
.define exp_b "dph"

.area REG_BANK_0	(REL,OVR,DATA)
rbank0:	.ds 8

ar0 = rbank0 + 0x00
ar1 = rbank0 + 0x01
ar2 = rbank0 + 0x02
ar3 = rbank0 + 0x03
ar4 = rbank0 + 0x04
ar5 = rbank0 + 0x05
ar6 = rbank0 + 0x06
ar7 = rbank0 + 0x07
