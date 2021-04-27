#include <stdio.h>
#include <unistd.h>

#include "networks.h"
#include "HandleTable.h"
#include "pduSendRecv.h"

#define FLAG1 1


void processSocket(int serverSocket);
int getHandle(int socket, char *handle);
void sendHandle(int socket, int pduLen, int flag);
int recieveFromClient(int clientSocket);
void printMpack(char *chatHeader, char *buff, int rcvlen);
void doneProcessingHandles(int clientSocket, int pduLen);
int checkFlag(int flag, int clientSocket);
void terminateClient(int clientSocket);
void processMessagePacket(int sendSocket, char *chatHeader, char *buff);
void processNumOfHandles(int clientSocket, int pduLen);
void processHandles(int clientSocket, int pduLen, int i);
void processBroadcast(int clientSocket, char *buff);
