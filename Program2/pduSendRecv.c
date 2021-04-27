#include "pduSendRecv.h"

int myRecieve(int clientSocket, char *chatHeader, char *buff)
{
  int mLen = 0;
  memset(buff, '\0', MAXBUF);

  //now get the data from the client_socket
  /* check to see if recv was succesful or if any closed connect occured.*/
  if ((mLen = recv(clientSocket, chatHeader, CHAT_HEADER_BYTE, MSG_WAITALL)) < 0)
  {
      perror("recv call");
      exit(-1);
  }
  else if (mLen == 0)
  {
      perror("closed connection");
      return -1;
  }

  //now get the data from the client_socket
  /* check to see if recv was succesful or if any closed connect occured.*/
  if ((mLen = recv(clientSocket, buff, MAXBUF, 0)) < 0)
  {
      perror("recv call");
      exit(-1);
  }
  else if (mLen == 0)
  {
      perror("closed connection");
      return -1;
  }
  return mLen;
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

void creatChatHeader(char *chatHeader, uint16_t pduLen, int flag)
{
  /* Convert header len to network byte order.*/
  uint16_t len = htons(pduLen);
  /* Copy 2 bytes of pduHeader*/
  memcpy(chatHeader, &len, PDU_LEN_BYTES);
  /* Copy 1 byte of flag data.*/
  memcpy(&chatHeader[PDU_LEN_BYTES], &flag, FLAG_BYTES);
}

void sendPacket(int socketNum, char *packet, int packetLen)
{
  if (send(socketNum, packet, packetLen, 0) < 0)
  {
    perror("send call");
    exit(-1);
  }
}
