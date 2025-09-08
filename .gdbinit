set mem inaccessible-by-default off
target remote localhost:3333 
b main
monitor reset
