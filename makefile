all: copy.c 
	gcc -o copy copy.c 

debug: copy.c 
	gcc -Wall -Wextra -o copy copy.c 

submission: copy.c makefile
	tar -czvf prog2.tgz copy.c makefile

clean: 
	rm copy

testClear:
	rm test.txt