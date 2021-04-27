#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

#include "pduSendRecv.h"
#include "pollLib.h"

#define M_HANDLE_LEN_BYTES 1
#define M_HANDLE_BYTES 13
#define M_NUM_DEST_BYTES 1
#define M_MESSAGE_BYTES 200

#define HANDLE_EXISTS 3
#define BROADCAST 4
#define MESSAGE_RECIVED 5
#define HANDLE_DOES_NOT_EXIST 7
#define LIST_ALL_HANDLES 11
#define NUM_OF_HANDLE 12
#define DONE_LISTING_HANDLES 13

void processClientSocket(char *handle, int socketNum);
void sendHandle(int socketNum, char *handle);
int recieveFromServer();
int sendServer(int socketNum);
int checkMessageFlag(char *message);
int readFromStdin(char * buffer);
int checkFlag(int flag, int clientSocket);
void unexistingHandle(char *handle);
void buildMessagePacket(int socket, char *packet, char *message);
void processRecievedMessage(char *buff);
void processNumOfHandles(char *buff);
void processRegisteredHandles(char *buff);
void buildBroadcastPacket(int socketNum, char *packet, char *message);
