# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019

CC= gcc
CFLAGS= -g -Wall
LIBS =
OBJ = safeUtil.o pollLib.o pduSendRecv.o ClientControl.o
OBJTWO = safeUtil.o pollLib.o pduSendRecv.o ServerControl.o HandleTable.o


all:   myClient myServer

myClient: myClient.c networks.o gethostbyname.o $(OBJ)
	$(CC) $(CFLAGS) -o cclient myClient.c networks.o gethostbyname.o $(OBJ)

myServer: myServer.c networks.o gethostbyname.o $(OBJTWO)
	$(CC) $(CFLAGS) -o server myServer.c networks.o gethostbyname.o $(OBJTWO)

.c.o:
	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f myServer myClient *.o
