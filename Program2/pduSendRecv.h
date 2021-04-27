#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/time.h>
#include "pollLib.h"

#define LEN_BYTES 2
#define PDU_LEN_BYTES 2
#define FLAG_BYTES 1
#define MAXBUF 1024
#define CHAT_HEADER_BYTE 3
#define FLAG_1 1
#define FLAG_2 2
#define FLAG_3 3
#define FLAG_4 4
#define FLAG_5 5
#define FLAG_6 6
#define FLAG_7 7
/* Exit Flag, Client to Server.*/
#define FLAG_8 8
/* Exit Flag, Server to Client.*/
#define FLAG_9 9
#define FLAG_10 10
#define FLAG_11 11
#define FLAG_12 12
#define FLAG_13 13

typedef struct PDUHeader
{
  uint16_t headerLen;
  uint8_t flag;
} __attribute__((packed)) PDUHeader;

int myRecieve(int clientSocket, char *chatHeader, char *buff);
void mySend(int socketNum, char *pdu, uint16_t pduLen);

void creatChatHeader(char *chatHeader, uint16_t pduLen, int flag);
void sendPacket(int socketNum, char *packet, int packetLen);
