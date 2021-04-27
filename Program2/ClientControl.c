#include "ClientControl.h"
char *clientHandle = NULL;

void processClientSocket(char *handle, int socketNum)
{
  clientHandle = handle;
  int newPollCall = 0;

  setupPollSet();
  /* Add Socket to the server.*/
  addToPollSet(socketNum);
  /* Add STDIN.*/
  addToPollSet(STDIN_FILENO);
  sendHandle(socketNum, handle);
  printf("$: ");
  fflush(stdout);

  while(1)
  {
    /* Block program until recieved a packet or STDIN.*/
    if((newPollCall = pollCall(-1)) != -1)
    {
      /* Poll Call detected a socket that is ready.*/
      if(newPollCall != 0)
      {
        /* Server sent us a packet.*/
        if(recieveFromServer(socketNum) < 0 )
        {
          break;
        }
      }
      else
      {
        /* Client is ready to send a message.*/
        sendServer(socketNum);
      }
    }

  }
printf("Closing\n");
fflush(stdout);
close(socketNum);
}

void sendHandle(int socketNum, char *handle)
{
  char packet[MAXBUF];
  int handleLen = strlen(handle) + 2;
  creatChatHeader(packet, handleLen + 2, FLAG_1);
  /* Create a messageHeader.*/
  memcpy(&packet[3], handle, handleLen);
  /* Sends packet to the server.*/
  sendPacket(socketNum, packet, handleLen + CHAT_HEADER_BYTE);
}


int checkFlag(int flag, int clientSocket)
{
  int exit = 0;

  switch(flag)
  {
    case FLAG_2:
        exit = 0;
        break;
    case FLAG_3:
        exit = FLAG_3;
        break;
    case FLAG_4:
        exit = FLAG_4;
        break;
    case FLAG_5:
         exit = FLAG_5;
         break;
    case FLAG_7:
        exit = FLAG_7;
        break;
    case FLAG_9:
        exit = -1;
        break;
    case FLAG_11:
         exit = FLAG_11;
         break;
    case FLAG_12:
        exit = FLAG_12;
        break;
    case FLAG_13:
        exit = FLAG_13;
        break;
    default:
        break;
  }
return exit;
}

void unexistingHandle(char *handle)
{
  printf("\nClient with handle %s does not exist\n", handle);
  printf("$: ");
  fflush(stdout);
}

/* Function that recives packet from the server.*/
int recieveFromServer(int socketNum)
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
  int flag = 0;
  int recvFlag = 0;

  /* Retrieve the packet from the client.*/
  if((rcvVal = myRecieve(socketNum, chatHeader, buff)) < 0)
  {
    perror("myRecieve call");
    return -1;
  }
  /* Extract the pdu Len from chat header. */
  memcpy(&pduLen, chatHeader, PDU_LEN_BYTES);
  /* Extract the chat flag from chat header.*/
  memcpy(&flag, &chatHeader[PDU_LEN_BYTES], FLAG_BYTES);
  /* Check for flag.*/
  if ((recvFlag = checkFlag(flag, socketNum)) < 0)
  {
    exit(-1);
  }

  if (recvFlag == HANDLE_EXISTS)
  {
    printf("Handle already in use: %s\n", clientHandle);
    fflush(stdout);
    exit(-1);
  }
  else if(recvFlag == MESSAGE_RECIVED || recvFlag == BROADCAST)
  {
    processRecievedMessage(buff);
    printf("$: ");
    fflush(stdout);
  }
  else if(recvFlag == HANDLE_DOES_NOT_EXIST)
  {
    unexistingHandle(buff);
  }
  else if (recvFlag == LIST_ALL_HANDLES)
  {
    processNumOfHandles(buff);
  }
  else if (recvFlag == NUM_OF_HANDLE)
  {
    processRegisteredHandles(buff);
  }
  else if(recvFlag == DONE_LISTING_HANDLES)
  {
    printf("$: ");
    fflush(stdout);
  }
return 0;
}

/* Function that sends packet to the server.*/
int sendServer(int socketNum)
{
  /* Buff to hold the entire packet structure.*/
  char packet[MAXBUF];
  /* Buff to hold message.*/
  char message[MAXBUF];
  /* the length of the message.*/
  int messageLen = 0;
  /* index to the message flag.*/
  int flagIdx = 0;
  int len = 0;
  memset(packet, 'F', MAXBUF);
  /* Read from STDIN.*/
  messageLen = readFromStdin(message);
  /* Get the index of the flag from the message recived from STDIN.*/
  if((flagIdx = checkMessageFlag(message)) < 0)
  {
    return -1;
  }
  /*TODO--------------REMOVE FLAG FROM Message!*/
  /* Create packet based on flag.*/
  switch(message[flagIdx])
  {
    /* Message falg, one to one or one to more than 1.*/
    case 'M':
    case 'm':
        /* builds the chat header.*/
        creatChatHeader(packet, messageLen + 2, FLAG_5);
        /* builds the message packet.*/
        buildMessagePacket(socketNum, packet, message);
        printf("$: ");
        fflush(stdout);
        break;

    case 'B':
    case 'b':
        len = messageLen + strlen(clientHandle) + 2;
        creatChatHeader(packet, len, FLAG_4);
        buildBroadcastPacket(socketNum, packet, message);
        sendPacket(socketNum, packet, MAXBUF);
        printf("$: ");
        fflush(stdout);
        break;
    case 'L':
    case 'l':
        creatChatHeader(packet, messageLen + 2, FLAG_10);
        memcpy(&packet[3], message, messageLen);
        sendPacket(socketNum, packet, CHAT_HEADER_BYTE + messageLen);
        break;
    /* Ending a connection %E client to server.*/
    case 'E':
    case 'e':
       /* Create a chat header.*/
       creatChatHeader(packet, messageLen + 2, FLAG_8);
       /* Create a messageHeader.*/
       memcpy(&packet[3], message, messageLen);
       /* Sends packet to the server.*/
       sendPacket(socketNum, packet, messageLen + CHAT_HEADER_BYTE);
       break;
  }

return 0;
}

/*Example Message: %M 1 handle1 hello how are you
  %m 2 fer luis hello*/
/*              Flag numDestHandles destHandle message.*/
void buildMessagePacket(int socket, char *packet, char *message)
{
  char outMess[MAXBUF];
  int messLen = 0;
  int handleLen = strlen(clientHandle);
  int destHandleLen = 0;
  int numDestHandles = 0;
  int offset = 0;
  int messageIdx = 5;
  char *destHandle = NULL;

  /* Get the number of destination handles. */
  numDestHandles = atoi(&message[3]);
  //printf("Number of destination handles: %d\n", numDestHandles);
  /* Only one dest handles.*/
  offset = 3;
  memcpy(&packet[offset], &handleLen, M_HANDLE_LEN_BYTES);
  offset += 1;

  memcpy(&packet[offset], clientHandle, M_HANDLE_BYTES);
  offset += 13;

  /* Adding number of destination handles.*/
  memcpy(&packet[offset], &numDestHandles, M_NUM_DEST_BYTES);
  offset += 1;

    int i;
    /* we have multiple dest handles.*/
    for(i = 0; i < numDestHandles; i++)
    {
      destHandle = NULL;
      /* Get the handle.*/
      destHandle = strtok(&message[messageIdx], " ");
      /* Compute the lenght of the handle.*/
      destHandleLen = strlen(destHandle);
      /* Message idx.*/
      messageIdx = messageIdx + destHandleLen + 1;

      memcpy(&packet[offset], &destHandleLen, M_HANDLE_LEN_BYTES);
      offset += 1;

      memcpy(&packet[offset], destHandle, M_HANDLE_BYTES);
      offset += 13;
    }
    memcpy(outMess, &message[messageIdx], M_MESSAGE_BYTES);

    if((messLen = strlen(outMess)) > 200)
    {
      outMess[messLen] = '\0';
      memcpy(&packet[offset], outMess, M_MESSAGE_BYTES);
      sendPacket(socket, packet, MAXBUF);
      memcpy(&packet[offset], &outMess[200], M_MESSAGE_BYTES);
      sendPacket(socket, packet, MAXBUF);
    }
    else
    {
      outMess[messLen] = '\0';
      memcpy(&packet[offset], outMess, M_MESSAGE_BYTES);
      sendPacket(socket, packet, MAXBUF);
    }
}

/* Function to check the flags of the message.
 * Returns flag idx when thier is a flag.
 * Returns -1 when no flag is found. */
int checkMessageFlag(char *message)
{
  /* If the message does not contain a % before flag.*/
  if(message[0] != '%')
  {
    printf("Invalid Command\n");
    printf("$: ");
    fflush(stdout);
    return -1;
  }

  if(message[1] == 'm' || message[1] == 'M')
  {
    if(atoi(&message[3]) > 9 )
    {
      printf("To many handle destinations\n");
      printf("$: ");
      fflush(stdout);
      return -1;
    }
  }

  return 1;
}

void processRecievedMessage(char *buff)
{
  int i;
  char message[MAXBUF];
  char handle1[MAXBUF];
  int offset = 0;
  int handleLen = 0;
  int numberDestination = 0;
  memcpy(&handleLen, buff, 1);
  offset += 1;
  char sendHandle[handleLen];
  memcpy(sendHandle, &buff[offset], 13);
  offset += 13;

  memcpy(&numberDestination,&buff[offset], M_NUM_DEST_BYTES);
  offset += 1;
  for(i = 0; i < numberDestination; i++)
  {
    memcpy(&handleLen, &buff[offset], M_HANDLE_LEN_BYTES);
    offset += 1;
    memcpy(handle1, &buff[offset], M_MESSAGE_BYTES);
    offset += 13;
  }
  memcpy(message, &buff[offset], M_MESSAGE_BYTES);
  printf("\n%s: %s\n", sendHandle, message);
  //printf("$: ");
  fflush(stdout);
}

void processNumOfHandles(char *buff)
{
  int numOfHandles = 0;
  memcpy(&numOfHandles, buff, 4);

  printf("Number of clients: %d\n", ntohs(numOfHandles));
}

void processRegisteredHandles(char *buff)
{
  char handle[MAXBUF];
  int handleLen = 0;

  memcpy(&handleLen, buff, 1);
  memcpy(handle, &buff[1], 100);
  printf(" %s\n", handle);

}

void buildBroadcastPacket(int socketNum, char *packet, char *message)
{
  char messToSend[MAXBUF];
  int messageLen = 0;
  int handleLen = strlen(clientHandle);
  int offset = 3;
  /* Add handle len to packet.*/
  memcpy(&packet[offset], &handleLen, 1);
  offset += 1;
  /* add client handle to packet*/
  memcpy(&packet[offset], clientHandle, 13);
  offset += 13;
  /* Get the message and add null terminating character.*/
  memcpy(messToSend, &message[3], 200);
  messageLen = strlen(messToSend);
  messToSend[messageLen] = '\0';
  /* Add message to packet.*/
  memcpy(&packet[offset], messToSend, 200);
}

/* Function from myClient.c, Will be using it.*/
int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;
	// Important you don't input more characters than you have space
	buffer[0] = '\0';
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}

	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;

	return inputLen;
}
