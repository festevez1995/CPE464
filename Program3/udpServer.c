/* Server side - UDP Code				    */
/* By Hugh Smith	4/1/2017	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "PDU.h"
#include "cpe464.h"
#include "serverControl.h"

#define MAXBUF 80

void processClient(int socketNum);
int checkArgs(int argc, char *argv[]);

int main ( int argc, char *argv[]  )
{
//	int socketNum = 0;
	int portNumber = 0;
	double errorRate = 0;

  if(argc < 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	errorRate = atof(argv[1]);
	portNumber = checkArgs(argc, argv);
	//socketNum = udpServerSetup(portNumber);

	/* A call to sendtoErr_init function*/
	sendErr_init(errorRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
	recieveClientRequest(portNumber);
	// processClient(socketNum);
	//
	// close(socketNum);

	return 0;
}

void processClient(int socketNum)
{
	int dataLen = 0;
	uint8_t buffer[MAXBUF + 1];
	struct sockaddr_in6 client;
	int clientAddrLen = sizeof(client);

	buffer[0] = '\0';
	while (buffer[0] != '.')
	{
		dataLen = safeRecvfrom(socketNum, buffer, MAXBUF, 0, (struct sockaddr *) &client, &clientAddrLen);

		printf("Received message from client with ");
		printIPInfo(&client);
		printf(" Len: %d \'%s\'\n", dataLen, buffer);

		/* Print the PDU buffer recived from the client.*/
		printPDU(buffer, dataLen);
		// just for fun send back to client number of bytes received
		//sprintf(buffer, "bytes: %d", dataLen);
		sendtoErr(socketNum, buffer, dataLen + 7, 0, (struct sockaddr *) & client, clientAddrLen);

	}
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 3)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}

	if (argc == 3)
	{
		portNumber = atoi(argv[2]);
	}

	return portNumber;
}
