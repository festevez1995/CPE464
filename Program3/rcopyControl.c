#include "rcopyControl.h"

void clientSetup(int argc, char *argv[], ClientData *client)
{
  if(argc != 8)
  {
    fprintf(stderr, "Usage:  %s from-filename to-filename window-size buffer-size error-percent remote-machine remote-port\n", argv[0]);
    exit(-1);
  }

  if(strlen(argv[1]) > 100)
  {
    fprintf(stderr, "%s to long. Maximum length 100 characters\n", argv[1]);
    exit(-1);
  }

  if(strlen(argv[2]) > 100)
  {
    fprintf(stderr, "%s to long. Maximum length 100 characters\n", argv[2]);
    exit(-1);
  }

  memcpy(client->from_filename, argv[1], 100);
  memcpy(client->to_filename, argv[2], 100);
  memcpy(client->remoteMachine, argv[6], strlen(argv[6]));
  client->windowSize = atoi(argv[3]);
  client->bufferSize = atoi(argv[4]);
  client->errorPercent = atof(argv[5]);
  client->remotePort = atoi(argv[7]);

}

void startProcessingRequests(ClientData *client)
{
  /* Set current state to FILE Name. get Ready to send data over.*/
  int curr_state = STATE_FILE_NAME;
  struct sockaddr_in6 server;		// Supports 4 and 6 but requires IPv6 struct
  int serverAddrLen = sizeof(struct sockaddr_in6);
  /* Create window*/
  Window window;
  /* Timer setup*/
  int timer = 0;
  int socketNum = 0;
  int fd = 0;
  setupPollSet();
  /* Repeat this until done.*/
  while(curr_state != TERMINATE)
  {
    if(timer > TIMEOUT)
    {
      curr_state = STATE_DONE;
    }
    switch (curr_state) {
      /* First State. Send the file name over to the server.*/
      case STATE_FILE_NAME:
          /* Connect tot the server.*/
            socketNum = setupUdpClientToServer(&server,
              client->remoteMachine, client->remotePort);
            /* Send the file name to the server and wait for its response.*/
            curr_state = handleInitFileName(socketNum, &server, serverAddrLen, client->from_filename,
              client->windowSize, client->bufferSize);
            /* If no response and timer runs out. We have a time out.*/
            if(curr_state == STATE_FILE_NAME)
            {
              /* Close socketNum and try to resend the file name.*/
              removeFromPollSet(socketNum);
              close(socketNum);
              /* Reste cur state.*/
              curr_state = STATE_FILE_NAME;
            }
            timer++;
            break;
      case STATE_FILE_OK:
          curr_state = fileOk(&fd, client->to_filename);
          break;
      case STATE_RECV_DATA:
          createWindow(&window, client->windowSize, client->bufferSize);
          curr_state = recieveData(socketNum, &server, serverAddrLen, &window);
          break;
      case STATE_EOF:
          break;
      case STATE_DONE:
          close(socketNum);
          curr_state = TERMINATE;
          break;
    }
  }
}

int handleInitFileName(int socket, struct sockaddr_in6 *server, int serverAddrLen, char *fileName, int windowSize, int buffSize)
{
  /* Create an empty buffer for payload.*/
  char payload[MAX_PAYLOAD];
  /* Create the size of the payload*/
  int payloadSize = 0;
  /* Offset variable used to offset payload.*/
  int offset = 0;
  /* Holds the state to return*/
  int state = 0;
  /* Copy the windowSize with size of 1 byte*/
  memcpy(payload, &windowSize, 1);
  /* Increment the offset by 1.*/
  offset += 1;
  /* Calcualte the payload size based on the windowSize.*/
  payloadSize += 1;
  /* Copy the buffsize onto the paylopad with 1 byte size.*/
  memcpy(&payload[offset], &buffSize, 4);
  /* Increment the offset.*/
  offset += 4;
  /* Calcualte the payload size with the new buffsize added.*/
  payloadSize += 4;
  /* Copy the filename onto the payload.*/
  memcpy(&payload[offset], fileName, strlen(fileName));
  offset += strlen(fileName);
  payload[offset] = '\0';
  /* Calculate the payload size with the new filename added.*/
  payloadSize += strlen(fileName) + 1;
  /* Call the sendFile fucntion and get its return state.*/
  state = sendFileName(socket, server, serverAddrLen, payload, payloadSize);
  //printInitPayload(payload);
return state;
}

int sendFileName(int socket, struct sockaddr_in6 *server, int serverAddrLen, char *payload, int payloadSize)
{
  //int serverAddrLen = sizeof(struct sockaddr_in6);
  uint8_t pdu[MAX_BUF];
  char data[MAX_BUF];
  char fileName[MAX_BUF];
  int pduLen = 0;
  int seqNum = 0;
  int flag = 0;


  pduLen = createPDU(pdu, seqNum, FLAG_7, payload, payloadSize);
  sendtoErr(socket, pdu, pduLen, 0, (struct sockaddr *) server, serverAddrLen);

  addToPollSet(socket);

  if(pollCall(1000) > 0)
  {
    safeRecvfrom(socket, data, MAX_BUF, 0, (struct sockaddr *) server, &serverAddrLen);

    memcpy(&flag, &data[6], 1);
    memcpy(fileName, &payload[2], payloadSize -2);

    if(flag == FLAG_8)
    {
      //printf("FILE OK STATE.\n");
      return STATE_FILE_OK;
    }
    else if(flag == FLAG_9)
    {
      printf("Error: file %s not found in the server.\n", fileName);
      return STATE_DONE;
    }

  }
return STATE_FILE_NAME;
}

int fileOk(int *fd, char *outFile)
{
  if((*fd = open(outFile, O_CREAT | O_TRUNC | O_WRONLY, 0600)) < 0)
  {
    perror("File open error");
    return STATE_DONE;
  }
  return STATE_RECV_DATA;
}

int recieveData(int socket, struct sockaddr_in6 *server, int serverAddrLen, Window *window)
{
  //int serverAddrLen = sizeof(struct sockaddr_in6);
  uint32_t seqNum = 0;
  uint16_t checksum = 0;

  int flag = 0;
  char payload[MAX_PAYLOAD];
  uint8_t data[MAX_BUF];
  int dataLen = 0;

  if(pollCall(10000) == 0)
  {
    printf("Select timeout\n");
    return STATE_DONE;
  }
  dataLen = safeRecvfrom(socket, data, MAX_BUF, 0, (struct sockaddr *) server, &serverAddrLen);
  memcpy(&seqNum, data, 4);
  seqNum = ntohl(seqNum);
  /* Extracts the flag from the pdu data*/
  memcpy(&flag, &data[6], 1);
  memcpy(payload, &data[7], MAX_PAYLOAD);

  if(flag == FLAG_EOF)
  {
    return STATE_EOF;
  }
  else if(flag == FLAG_3)
  {
    addBuffToWindow(window, data, dataLen);
    checksum = in_cksum((unsigned short *) data, dataLen);
    // printf("Len: %d\n", dataLen);
    // printf("Checksum: %d", checksum);
    // fflush(stdout);
    // if (checksum == 0)
    // {
    //   processSequnceNumber(window, data, dataLen);
    // }
    // else
    // {
    //   /*TODO Send SREJ*/
    // }
    return STATE_RECV_DATA;
  }
return STATE_RECV_DATA;
}

void processSequnceNumber(Window *window, uint8_t *data, int dataLen)
{

}
