#include "pduSendRecv.h"

void mySend(int socketNum)
{
  char pdu[MAXBUF];       // PDU buff to hold PDU length and payload
  char sendBuf[MAXBUF];   //data buffer
  char recvBuff[MAXBUF];  // data buffer for recieving messages
  int messageLen = 0;
	int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */
  int pduLen = 2;          // 2 Bytes of len of header.

  /* Run loop unitl exit key word is recived.*/
  while(strcmp(sendBuf, "exit") != 0)
  {
      sendLen = readFromStdin(sendBuf);
      pduLen += sendLen; //Calculating pduLen

      memcpy(pdu, &pduLen, PDU_LEN_BYTES);
      memcpy(&pdu[2], sendBuf, sendLen);
      /* Send the PDU block.*/
	    if ((sent = send(socketNum, pdu, pduLen, 0)) < 0)
	    {
		      perror("send call");
		      exit(-1);
	    }
      /* Recieve back message 1. Recieve message*/
      if((messageLen = recv(socketNum, recvBuff, pduLen, MSG_WAITALL)) < 0)
      {
          perror("recv call");
          exit(-1);
      }
	    printf("Recv() from server: %s\n", recvBuff);

      /* Recieve back message 2. Recieve message*/
      if((messageLen = recv(socketNum, recvBuff, pduLen, 0)) < 0)
      {
          perror("recv call");
          exit(-1);
      }
	    printf(
        "Recv() from server: Number of bytes recieved by the server was %s\n", recvBuff);
      /* reset variables.*/
      sendLen = 0;
      pduLen = 2;
      sent = 0;
  }
  /* On exit, send the exit message.*/
  /* pduLen is 2 bytes pluse the size of exit including the null character.*/
  pduLen = 2 + 5;
  memcpy(pdu, &pduLen, PDU_LEN_BYTES);
  memcpy(&pdu[2], "exit\0", sendLen);
  if ((sent = send(socketNum, pdu, pduLen, 0)) < 0)
  {
      perror("send call");
      exit(-1);
  }
}

/* Server recieves message from the client.
 */
int myRecive(int clientSocket)
{
  char buff[MAXBUF];
  char str[MAXBUF];
	int messageLen = 0;
  int pduLen = 0;
  int sent = 0;
  /* variables used for select().*/
  fd_set readfds;
  struct timeval t;


  while(strcmp(buff, "exit") != 0)
  {
      t.tv_sec = 1;
      t.tv_usec = 0;
      FD_ZERO(&readfds);
      FD_SET(clientSocket, &readfds);

      /* Block until recieve the message.
       * Note: In class notes were used to implement this block of code.*/
      if(select(clientSocket + 1, &readfds, NULL, NULL, &t) < 0)
      {
        perror("select call");
        exit(-1);
      }

      messageLen = recv(clientSocket, &pduLen, LEN_BYTES, MSG_WAITALL);
      /* check to see if recv was succesful or if any closed connect occured.*/
	    if (messageLen < 0)
	    {
		      perror("recv call");
		      exit(-1);
	    }
      else if (messageLen == 0)
      {
          perror("closed connection");
          exit(-1);
      }

      messageLen = recv(clientSocket, buff, MAXBUF, 0);
      //now get the data from the client_socket
      /* check to see if recv was succesful or if any closed connect occured.*/
	    if (messageLen < 0)
	    {
		      perror("recv call");
		      exit(-1);
	    }
      else if (messageLen == 0)
      {
          perror("closed connection");
          exit(-1);
      }
	    printf("recv() Len: %d, PDU Len: %d, Message: %s\n",
              messageLen + 2, pduLen, buff);
      sprintf(str, "%d", messageLen);
      /* Sending back the text message*/
      if((sent = send(clientSocket, buff, pduLen, 0)) < 0)
      {
          perror("sent error");
          exit(-1);

      }
      /* Sending number of bytes recieved by the server.*/
      if((sent = send(clientSocket, str, pduLen, 0)) < 0)
      {
          perror("sent 2nd PDU error");
          exit(-1);
      }
  }
  return 0;
}

/* Function form myClient.c
 * using this function for my .c file. */
int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;

	// Important you don't input more characters than you have space
	buffer[0] = '\0';
	printf("Enter data: ");
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
