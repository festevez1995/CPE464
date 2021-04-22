#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/time.h>
#include "pollLib.h"

#define LEN_BYTES 2
#define PDU_LEN_BYTES 2
#define MAXBUF 1024

int myRecieve(int clientSocket);
void mySend(int socketNum, char *pdu, uint16_t pduLen);
