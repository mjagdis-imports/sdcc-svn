set error non-classified off
set error unknown_code off
set error memory off
set error stack off
set hw simif outputs 0xff
show con
run
state
show con
kill
quit
