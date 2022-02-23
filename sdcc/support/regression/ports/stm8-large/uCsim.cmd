set error non-classified off
set error unknown_code off
set error memory off
set error stack off
set hw simif rom 0x7fff
uart1_on=0
uart3_on=0
run
state
quit
