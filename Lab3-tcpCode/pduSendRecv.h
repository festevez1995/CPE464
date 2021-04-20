#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/time.h>

#define LEN_BYTES 2
#define PDU_LEN_BYTES 2
#define MAXBUF 1024


void mySend(int socketNum);
int myRecive(int clientSocket);
int readFromStdin(char * buffer);
