#include "serverControl.h"

void recieveClientRequest(int portNumber)
{
  int socketNum = 0;
  int dataLen = 0;
  uint8_t data[MAX_BUF];
  Connection server;

  socketNum = udpServerSetup(portNumber);

  server.socketNum = socketNum;

  setupPollSet();
  addToPollSet(socketNum);
  while(1)
  {
    /* Ready to read from client.*/
    if(pollCall(1000) > 0)
    {
      dataLen = safeRecvfrom(socketNum, data, MAX_BUF, 0, (struct sockaddr *) &server.remote, &server.addrLen);

      processClientRequest(&server, data, dataLen);

      break;

    }
  }
  close(socketNum);
}

void parseRecievedMessage(uint8_t *buff, char *payload)
{
  int offset = 7;
  memcpy(payload, &buff[offset], MAX_PAYLOAD);
}

void getData(int *windowSize, int *buffSize, uint8_t *data)
{
  char payload[MAX_PAYLOAD];
  int offset = 7;
  int winSize = 0;
  int bsize = 0;

  memcpy(payload, &data[offset], MAX_PAYLOAD);

  memcpy(&winSize, payload, 1);
  memcpy(&bsize, &payload[1], 4);
  *windowSize = winSize;
  *buffSize = bsize;
}

void processClientRequest(Connection *server, uint8_t *data, int dataLen)
{
  int state = START;
  int fd = 0;
  int windowSize = 0;
  int buffSize = 0;
  int sequenceNumber = 0;
  Window window;

  while(state != TERMINATE)
  {
    switch (state) {
      case START:
          getData(&windowSize, &buffSize, data);
          state = STATE_FILE_NAME;
          break;
      case STATE_FILE_NAME:
          state = processRecievedMessage(server, data, dataLen, &fd);
          break;
      case STATE_SEND_DATA:
          createWindow(&window, windowSize, buffSize);
          state = prepareData(server, &window, fd, buffSize, &sequenceNumber);
          break;
      case STATE_ACK_WAIT:
          break;
      case STATE_EOF:
          break;
      case STATE_DONE:
          state = TERMINATE;
          close(server->socketNum);
          break;
    }
  }
}

int processRecievedMessage(Connection *server, uint8_t *data, int dataLen, int *fd)
{
  char payload[MAX_PAYLOAD];
  char fileName[100];
  int flag = 0;
  /* Copy flag from PDU.*/
  memcpy(&flag, &data[FLAG_IDX], 1);
  /* Get the payload buffer*/
  parseRecievedMessage(data, payload);
  printInitPayload(payload);
  /* Copy filename from payload buff.*/
  memcpy(fileName, &payload[5], 100);

  if(((*fd) = open(fileName, O_RDONLY)) < 0)
  {
    flag = FLAG_BAD_FILENAME;
    memcpy(&data[FLAG_IDX], &flag, 1);
    sendtoErr(server->socketNum, data, dataLen, 0, (struct sockaddr *)&server->remote, server->addrLen);
    return STATE_DONE;
  }

  flag = FLAG_GOOD_FILENAME;
  memcpy(&data[FLAG_IDX], &flag, 1);
  sendtoErr(server->socketNum, data, dataLen, 0, (struct sockaddr *)&server->remote, server->addrLen);
  return STATE_SEND_DATA;
}

int prepareData(Connection *server, Window *window, int fd, int buffSize, int *sequenceNumber)
{
  char fileBuff[MAX_BUF];
  uint8_t pdu[MAX_BUF];
  int pduLen = 0;
  int dataLen = -1;
  int state = STATE_DONE;

  //memset(fileBuff, '\0', MAX_PAYLOAD);
  if((dataLen = read(fd, fileBuff, buffSize)) < 0)
  {
    perror("read error");
    return STATE_DONE;
  }
  else if(dataLen == 0)
  {
    return STATE_EOF;
  }
  else
  {
    fileBuff[buffSize + 1] = '\0';
    pduLen = createPDU(pdu, *sequenceNumber, FLAG_DATA_PACKET, fileBuff, buffSize + 1);
    (*sequenceNumber)++;
    addBuffToWindow(window, pdu, pduLen);
    state = sendDataPacket(server, window, pdu, pduLen, FLAG_DATA_PACKET);
  }
return state;
}

int sendDataPacket(Connection *server, Window *window, uint8_t *pdu, int pduLen, int flag)
{
  if(isWindowClosed(window) == 0)
  {
    /*TODO fix this*/
    return STATE_DONE;
  }
  sendtoErr(server->socketNum, pdu, pduLen, 0, (struct sockaddr *)&server->remote, server->addrLen);

  return STATE_ACK_WAIT;
}
