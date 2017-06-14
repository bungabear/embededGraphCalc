calc: calc.o queue.o
	gcc -o calc calc.o queue.o
calc.o: calc.c queue.h
	gcc -c calc.c
queue.o: queue.c queue.h
	gcc -c queue.c
