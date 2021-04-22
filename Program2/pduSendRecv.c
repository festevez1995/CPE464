#include "pduSendRecv.h"

int myRecieve(int clientSocket)
{
  char buff[MAXBUF];
  char sendBuff[MAXBUF];
  char str[MAXBUF];
  int pduLen = 0;
  int mLen = 0;
  int sent = 0;            //actual amount of data sent/* get the data and send it   */

  memset(buff, '\0', MAXBUF);

  //now get the data from the client_socket
  /* check to see if recv was succesful or if any closed connect occured.*/
  if ((mLen = recv(clientSocket, &pduLen, LEN_BYTES, MSG_WAITALL)) < 0)
  {
      perror("recv call");
      exit(-1);
  }
  else if (mLen == 0)
  {
      perror("closed connection");
      return -1;
  }
  pduLen = ntohs(pduLen);

  //now get the data from the client_socket
  /* check to see if recv was succesful or if any closed connect occured.*/
  if ((mLen = recv(clientSocket, buff, pduLen - 2, 0)) < 0)
  {
      perror("recv call");
      exit(-1);
  }
  else if (mLen == 0)
  {
      perror("closed connection");
      return -1;
  }

  printf("recv() Len: %d, PDU Len: %d, Message: %s\n", mLen+2, pduLen, buff);

  char mess[50] = "Number of bytes recived by the server was ";
  memcpy(sendBuff, mess, strlen(mess) + 1);
  sprintf(str, "%d", mLen);
  memcpy(sendBuff + strlen(sendBuff), str, strlen(str) + 1);

  if ((sent = send(clientSocket, buff, pduLen, 0)) < 0)
  {
    perror("send call");
    exit(-1);
  }

  if ((sent = send(clientSocket, sendBuff, MAXBUF, 0)) < 0)
  {
    perror("send call");
    exit(-1);
  }

  return 0;
}

/* My impementation for send. */
void mySend(int socketNum, char *pdu, uint16_t pduLen)
{
  char recvBuff[MAXBUF];  // data buffer for recieving messages

  if (send(socketNum, pdu, pduLen, 0) < 0)
  {
    perror("send call");
    exit(-1);
  }
  /* Recieve back message 1. Recieve message*/
  if(recv(socketNum, recvBuff, pduLen, MSG_WAITALL) < 0)
  {
    perror("recv call");
    exit(-1);
  }
	printf("Recv() from server: %s\n", recvBuff);

  /* Recieve back message 2. Recieve message*/
  if(recv(socketNum, recvBuff, MAXBUF, 0) < 0)
  {
    perror("recv call");
    exit(-1);
  }
	printf("Recv() from server: %s\n", recvBuff);

}
