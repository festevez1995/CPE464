# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019

CC= gcc
CFLAGS= -g -Wall
LIBS =


all:   myClient myServer

myClient: myClient.c safeUtil.o pollLib.o pduSendRecv.o networks.o gethostbyname.o
	$(CC) $(CFLAGS) -o cclient myClient.c safeUtil.o pollLib.o pduSendRecv.o networks.o gethostbyname.o $(LIBS)

myServer: myServer.c safeUtil.o pollLib.o pduSendRecv.o networks.o gethostbyname.o
	$(CC) $(CFLAGS) -o server myServer.c safeUtil.o pollLib.o pduSendRecv.o networks.o gethostbyname.o $(LIBS)

.c.o:
	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f myServer myClient *.o
