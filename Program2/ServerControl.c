#include "ServerControl.h"
#define DEBUG_FLAG 1

void processSocket(int serverSocket)
{
  int clientSocket = 0;
  int acceptCode = 0;
  int code = 0;

  createHTable();

  setupPollSet();
  addToPollSet(serverSocket);
  // Recieves multiple clients
  while(1)
	{

	 // wait for client to connect
		 if ((clientSocket = pollCall(-1)) !=  -1)
		 {
		     if(clientSocket == serverSocket)
		     {
               char handle[MAXBUF];
               acceptCode = tcpAccept(serverSocket, DEBUG_FLAG);
               if(getHandle(acceptCode, handle) != -1)
               {
                 addToHTable(handle, acceptCode);
		             addToPollSet(acceptCode);
               }
		     }
		     else
		     {
	           if((code = recieveFromClient(clientSocket)) < 0)
	           {
                 /* Delete clientSocket from Handle Table.*/
                 deleteFromHTable(clientSocket);
				         removeFromPollSet(clientSocket);
			           /* close the sockets */
			           close(clientSocket);
	           }
		     }
		 }
  }
  close(serverSocket);
}

/* Todo - CHECK IF handle name all ready exists in the htable
 * TODO - SEND back to client confirming a good handle.*/
int getHandle(int socket, char *handle)
{
  /* Holdes Chat header block*/
  char chatHeader[MAXBUF];
  int pduLen = 0;
  uint8_t flag = 0;
  int mess = 0;

  if((mess = myRecieve(socket, chatHeader, handle)) < 0)
  {
    perror("myRecieve call");
    exit(-1);
  }

  memcpy(&pduLen, chatHeader, PDU_LEN_BYTES);
  pduLen = ntohs(pduLen);
  memcpy(&flag, chatHeader + 2, FLAG_BYTES);

  /* Check for flag.*/
  /*Chcking flags now. */
  if (checkFlag(flag, socket) < 0)
  {
    return -1;
  }
  printf("Recv len: %d Msg Len: %d flag: %d", mess + 2, pduLen, flag);
  printf(" (srcLen: %lu srcHandle: %s)\n", strlen(handle), handle);
  fflush(stdout);
  if (handleExist(handle) < 0)
  {
    sendHandle(socket, pduLen, FLAG_2);
    return 0;
  }
  else
  {
    printf("Handle all ready used\n");
    sendHandle(socket, pduLen, FLAG_3);
    return -1;
  }
}
void sendHandle(int socket, int pduLen, int flag)
{
  char chatHeader[MAXBUF];
  /* Create a chat header.*/
  creatChatHeader(chatHeader,pduLen, flag);
  /* Create am empty messageHeader.*/
  memcpy(&chatHeader[3], " ", 1);
  /* Sends packet to the server.*/
  sendPacket(socket, chatHeader, 1 + CHAT_HEADER_BYTE);
}
/* Function that recives message from client.
 * Return -1 upon failure
 * Return 0 on success.*/
int recieveFromClient(int clientSocket)
{
  /* Holdes Chat header block*/
  char chatHeader[MAXBUF];
  /* Holds message */
  char buff[MAXBUF];
  /* Use to check myRecieve error.*/
  int rcvVal = 0;
  /* Value to hold the pduLen of packet.*/
  int pduLen = 0;
  /* Vlue to hold flag len of packet.*/
  uint8_t flag = 0;
  int retFlag = 0;
  /* Retrieve the packet from the client.*/
  if((rcvVal = myRecieve(clientSocket, chatHeader, buff)) < 0)
  {
    perror("myRecieve call");
    return -1;
  }
  /* Extract the pdu Len from chat header. */
  memcpy(&pduLen, chatHeader, PDU_LEN_BYTES);
  pduLen = ntohs(pduLen);
  /* Extract the chat flag from chat header.*/
  memcpy(&flag, chatHeader + 2, FLAG_BYTES);

  /* Check for flag errors.*/
  if ((retFlag = checkFlag(flag, clientSocket)) < 0)
  {
    printf("Recv: %d Msg Len: %d flag: %d\n", rcvVal, pduLen, flag);
    fflush(stdout);
    return -1;
  }
  if (retFlag == FLAG_4)
  {
    processBroadcast(clientSocket, buff);
  }
  else if (retFlag == FLAG_5)
  {
    processMessagePacket(clientSocket, chatHeader, buff);
  }
  else if (retFlag == FLAG_10)
  {
    printf("Recv: %d Msg Len: %d flag: %d\n", rcvVal, pduLen, flag);
    fflush(stdout);
    processNumOfHandles(clientSocket, pduLen);
  }
  return 0;
}

void printMpack(char *chatHeader, char *buff, int rcvlen)
{
  int pduLen = 0;
  int flag = 0;
  int handleLen = 0;
  int destNum = 0;
  char handle[MAXBUF];
  char message[MAXBUF];
  int offset = 0;
  int i;
  memcpy(&pduLen, chatHeader, PDU_LEN_BYTES);
  pduLen = ntohs(pduLen);
  memcpy(&flag, chatHeader + 2, FLAG_BYTES);

  memcpy(&handleLen, buff, 1);
  offset += 1;
  memcpy(handle, &buff[offset], 13);
  offset += 13;
  memcpy(&destNum, &buff[offset], 1);
  offset += 1;
  printf("Recv len: %d Msg Len: %d flag: %d", rcvlen, pduLen, flag);
  printf(" (scrLen: %d srcHandle: %s) numDst: %d", handleLen, handle, destNum);
  for(i = 0; i < destNum; i++)
  {
    memcpy(&handleLen, &buff[offset], 1);
    offset += 1;
    memcpy(handle, &buff[offset], 13);
    offset += 13;
    printf("( (destLen: %d destHandle: %s)\n", handleLen, handle);
  }
  int messLen = 0;
  memcpy(message, &buff[offset], 200);
  messLen = strlen(message);

  printf(" message strlen len: %d mesg: %s", messLen, message);
  fflush(stdout);
}


int checkFlag(int flag, int clientSocket)
{
  int exit = 0;

  switch(flag)
  {
    case FLAG_1:
        //printf("Flag 1\n");
        exit = 0;
        break;
    case FLAG_4:
        exit = FLAG_4;
        break;
    case FLAG_5:
        //printf("Flag 5 has been recived\n");
        exit = FLAG_5;
        break;
    case FLAG_8:
        exit = -1;
        terminateClient(clientSocket);
        break;
    case FLAG_10:
        exit = FLAG_10;
        break;
    default:
        exit = -1;
        break;
  }
return exit;
}
/* Terminates a client.*/
void terminateClient(int clientSocket)
{
  /* Contains chat header with flag */
  char buff[MAXBUF];
  /* Create a packet to send to client.*/
  creatChatHeader(buff, 0 + 2, FLAG_9);
  /* Send the packet to the client.*/
  sendPacket(clientSocket, buff, CHAT_HEADER_BYTE );
  //close(clientSocket);
}
/* Chat header, handleLen , sending handle, numDestHandles, destHandleLen,
 * destHandle, message*/
void processMessagePacket(int sendSocket, char *chatHeader, char *buff)
{
  char packet[MAXBUF];
  char destHandle[MAXBUF];
  int numDestHandles = 0;
  int destLen = 0;
  int pduLen = 0;
  /*Starts on numberDestination Handles.*/
  int offset = 14;
  int i;
  /* Extract the pdu Len from chat header. */
  memcpy(&pduLen, chatHeader, PDU_LEN_BYTES);
  pduLen = ntohs(pduLen);

  /*Set chat header to packet.*/
  memcpy(packet, chatHeader, CHAT_HEADER_BYTE);
  /* Add message portion of block.*/
  memcpy(&packet[CHAT_HEADER_BYTE], buff, MAXBUF - 3);

  memcpy(&numDestHandles, &buff[offset], 1);
  /* Offset by 2 because we are ignoring the destination handle len.*/
  offset+=1;
  for(i = 0; i < numDestHandles; i++)
  {
    /*Offset by 1 because we are not considering handle len.*/
    memcpy(&destLen, &buff[offset], 1);
    offset += 1;
    /* Get the destination handle.*/
    memcpy(destHandle, &buff[offset], 13);
    offset += 13;
    /* Check if handle exists in the hTable.*/
    if(handleExist(destHandle) == 0)
    {
      int socketToSend = getFromHTable(destHandle);
      sendPacket(socketToSend, packet, MAXBUF);
    }
    /* if it doesn't send flag 7 to sending client.*/
    else
    {
      char packet[MAXBUF];
      creatChatHeader(packet, pduLen, FLAG_7);
      /* Create a messageHeader.*/
      memcpy(&packet[3], destHandle, destLen);
      sendPacket(sendSocket, packet, CHAT_HEADER_BYTE + destLen);
    }
  }
}

void processNumOfHandles(int clientSocket, int pduLen)
{
  /*Packet block to send back to the client.*/
  char packet[MAXBUF];
  int i;
  /* Gets the total number of enteries in the Htable.*/
  int numOfHandles = getNumOfHandle();
  /* Create the chat header.*/
  creatChatHeader(packet, pduLen, FLAG_11);
  /* COnvert num of handle to network byte order.*/
  numOfHandles = htons(numOfHandles);
  /* Copy it to the packet.*/
  memcpy(&packet[3], &numOfHandles, 4);
  /* Send out the packet to the client. */
  sendPacket(clientSocket, packet, CHAT_HEADER_BYTE + numOfHandles);

  for(i = 0; i < ntohs(numOfHandles); i++)
  {
    /*Process the handles. */
    processHandles(clientSocket, pduLen, i);
  }
  doneProcessingHandles(clientSocket, pduLen);
}

void processHandles(int clientSocket, int pduLen, int i)
{
  /*Packet block to send back to the client.*/
  char packet[MAXBUF];
  int handleLen = 0;
  /* Create the chat header.*/
  creatChatHeader(packet, pduLen, FLAG_12);

  handleLen = strlen(getHandleHTable(i));
  memcpy(&packet[3], &handleLen, 1);
  memcpy(&packet[4], getHandleHTable(i), 100);
  sendPacket(clientSocket, packet, CHAT_HEADER_BYTE + 1 + handleLen);

}

void doneProcessingHandles(int clientSocket, int pduLen)
{
  char done[MAXBUF];
  int flag = DEBUG_FLAG;
  creatChatHeader(done, pduLen, FLAG_13);
  /* Placed a debug flag.*/
  memcpy(&done[3], &flag, 1);
  sendPacket(clientSocket, done, CHAT_HEADER_BYTE + 1);
}

void processBroadcast(int clientSocket, char *buff)
{
  char packet[MAXBUF];
  char newBuff[MAXBUF];
  char message[MAXBUF];
  char handle[MAXBUF];
  int pduLen = 1;
  int handleLen = 0;
  int offset = 0;
  int num = getNumOfHandle();
  int i;

  /*Get the handle len from packet.*/
  memcpy(&handleLen, buff, 1);
  offset += 1;
  /* Get the sending handle from packet.*/
  memcpy(handle, &buff[offset], 13);
  offset += 13;
  memcpy(message, &buff[offset], 200);
  /* Chat header, handleLen , sending handle, numDestHandles, destHandleLen,
   * destHandle, message*/
   /*prepare buff.*/
  offset = 0;
  memcpy(newBuff, &handleLen, 1);
  offset +=1;
  pduLen += handleLen;

  memcpy(&newBuff[offset], handle, 13);
  offset += 13;

  int numDest = 1;
  memcpy(&newBuff[offset], &numDest, 1);
  offset += 1;
  pduLen += 1;

  int resetLen = pduLen;
  int resOffset = offset;
//  printf("Before Offset: %d, len: %d\n", resOffset, pduLen);
  /* Get each cleint from table and broadcast the message*/
  for(i = 0; i < num; i++)
  {
    if(strcmp(handle, getHandleHTable(i)) != 0)
    {
      handleLen = strlen(getHandleHTable(i));
      memcpy(&newBuff[offset], &handleLen, 1);
      offset += 1;
      pduLen += handleLen;

      memcpy(&newBuff[offset], getHandleHTable(i), 13);
      offset += 13;

      memcpy(&newBuff[offset], message, 200);
      pduLen += strlen(message);

      creatChatHeader(packet, pduLen + 2, FLAG_4);
  //    printf("Handle Len: %d, Handle: %s\n", handleLen, getHandleHTable(i));
    //  printf("Message: %s\n", message);
      //fflush(stdout);
      processMessagePacket(clientSocket, packet, newBuff);
      pduLen = resetLen;
      offset = resOffset;
    }
    //printf("After Offset: %d, len: %d\n", resOffset, pduLen);

  }
  //pduLen = 1 + handleLen + 1 + destLen + strlen(message);

}
