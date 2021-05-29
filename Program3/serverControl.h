#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "pollLib.h"
#include "PDU.h"
#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "cpe464.h"
#include "WindowLib.h"

/* States for server*/
#define START 0
#define STATE_FILE_NAME 1
#define STATE_SEND_DATA 2
#define STATE_ACK_WAIT 3
#define STATE_EOF 4
#define STATE_DONE 5
#define TERMINATE 6

/*Flags for the server.*/
#define FLAG_DATA_PACKET 3
#define FLAG_RR_PACKET 5
#define FLAG_SREJ_PACKET 6
#define FLAG_FILE_NAME 7
#define FLAG_GOOD_FILENAME 8
#define FLAG_BAD_FILENAME 9
#define FLAG_EOF 10

/*PDU Idx's*/
#define SEQ_NUM_IDX 0
#define CHECKSUM_IDX 4
#define FLAG_IDX 6
#define PAYLOAD_IDX 7

#define MAX_BUF 2800

typedef struct Connection
{
  int socketNum;
  struct sockaddr_in6 remote;
  int addrLen;
} Connection;

void recieveClientRequest(int portNumber);
void parseRecievedMessage(uint8_t *buff, char *payload);
void getData(int *windowSize, int *buffSize, uint8_t *data);
void processClientRequest(Connection *server, uint8_t *data, int dataLen);
int processRecievedMessage(Connection *server, uint8_t *data, int dataLen, int *fd);
int prepareData(Connection *server, Window *window, int fd, int buffSize, int *sequenceNumber);
int sendDataPacket(Connection *server, Window *window, uint8_t *pdu, int pduLen, int flag);
