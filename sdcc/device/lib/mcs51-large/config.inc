; assembler configuration
.optsdcc -mmcs51 --model-large

; defines for conditional code
.define MCS51
.define MODEL_LARGE

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

