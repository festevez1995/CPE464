#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "WindowLib.h"
#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "PDU.h"
#include "cpe464.h"
#include "pollLib.h"

#define TIMEOUT 10
/* States for FSM*/
#define STATE_FILE_NAME 1
#define STATE_FILE_OK 2
#define STATE_RECV_DATA 3
#define STATE_DONE 4
#define STATE_EOF 5
#define TERMINATE -1

#define FLAG_3 3
#define FLAG_5 5
#define FLAG_7 7
#define FLAG_8 8
#define FLAG_9 9
#define FLAG_EOF 10

#define MAX_BUF 2800
typedef struct ClientData
{
  char from_filename[100];
  char to_filename[100];
  int windowSize;
  int bufferSize;
  double errorPercent;
  char remoteMachine[100];
  int remotePort;
} ClientData;

void clientSetup(int argc, char *argv[], ClientData *client);
void startProcessingRequests(ClientData *client);

int handleInitFileName(int socket, struct sockaddr_in6 *server, int serverAddrLen, char *fileName, int windowSize, int buffSize);
int sendFileName(int socket, struct sockaddr_in6 *server, int serverAddrLen, char *payload, int payloadSize);
int fileOk(int *fd, char *outFile);
int recieveData(int socket, struct sockaddr_in6 *server,int serverAddrLen, Window *window);
void processSequnceNumber(Window *window, uint8_t *data, int dataLen);
