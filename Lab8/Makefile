CC = gcc
CFLAGS = -g -Wall


SRC = checksum.c PDU.c WindowLib.c
OBJS = checksum.o PDU.o WindowLib.o

all: simpleTest

simpleTest: simpleTest.c $(OBJS)
	$(CC) $(CFLAGS) -o simpleTest simpleTest.c  $(OBJS) 

%.o: %.c *.h
	gcc -c $(CFLAGS) $< -o $@

cleano:
	rm -f *.o

clean:
	rm -f udpServer udpClient *.o
