CC = gcc
OBJECTS = calc.o queue.o pointqueue.o
CFLAGS = -g -c
 
TARGET = calc 
 
$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)
               
clean : 
	rm -f $(OBJECTS) $(TARGET)
#calc: calc.o queue.o
#	gcc -o calc calc.o queue.o
#calc.o: calc.c queue.h
#	gcc -c calc.c
#queue.o: queue.c queue.h
#	gcc -c queue.c
#pointqueue.o: pointqueue.c pointqueue.h
#    gcc -c pointqueue.c
