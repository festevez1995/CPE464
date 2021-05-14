// Client side - UDP Code
// By Hugh Smith	4/1/2017

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "PDU.h"
#include "cpe464.h"

#define MAXBUF 80

void talkToServer(int socketNum, struct sockaddr_in6 * server);
int readFromStdin(char * buffer);
int checkArgs(int argc, char * argv[]);


int main (int argc, char *argv[])
 {
	int socketNum = 0;
	struct sockaddr_in6 server;		// Supports 4 and 6 but requires IPv6 struct
	int portNumber = 0;
  double errorRate = 0;

  errorRate = atof(argv[1]);

	portNumber = checkArgs(argc, argv);

	socketNum = setupUdpClientToServer(&server, argv[2], portNumber);
  /* A call to sendtoErr_init function*/
  sendErr_init(errorRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);

	talkToServer(socketNum, &server);

	close(socketNum);

	return 0;
}

void talkToServer(int socketNum, struct sockaddr_in6 * server)
{
	int serverAddrLen = sizeof(struct sockaddr_in6);
	char * ipString = NULL;
	int dataLen = 0;
	char buffer[MAXBUF+1];
  /* Buffer that will hold the PDU.*/
  uint8_t pduBuff[MAXBUF];
  /* Sequence number counter. Proof of concept.*/
  uint32_t sequenceNum = 0;
  int pduLen = 0;
	buffer[0] = '\0';
	while (buffer[0] != '.')
	{
		dataLen = readFromStdin(buffer);

		//printf("Sending: %s with len: %d\n", buffer,dataLen);
    /* Creates a pdu header.*/
    pduLen = createPDU(pduBuff, sequenceNum++, PDU_TEST_FLAG, buffer, dataLen);

    /* Varifies that the pdu header is working.*/
    printPDU(pduBuff, pduLen);
    /* Changing all the sendto to sendtoErr function.*/
		//safeSendto(socketNum, buffer, dataLen, 0, (struct sockaddr *) server, serverAddrLen);
    sendtoErr(socketNum, pduBuff, pduLen, 0, (struct sockaddr *) server, serverAddrLen);

		safeRecvfrom(socketNum, buffer, MAXBUF, 0, (struct sockaddr *) server, &serverAddrLen);

		// print out bytes received
		ipString = ipAddressToString(server);
		printf("Server with ip: %s and port %d said it received %s\n", ipString, ntohs(server->sin6_port), buffer);

	}
}

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

int checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}

	// Checks args and returns port number
	int portNumber = 0;

	if (argc != 4)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}

	portNumber = atoi(argv[3]);
  printf("%d\n", portNumber);

	return portNumber;
}
