/******************************************************************************
* tcp_server.c
*
* CPE 464 - Program 1
*****************************************************************************/

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

#include "networks.h"
#include "pduSendRecv.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

void recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
  int code = 0;
	struct sockaddr socketAddr;
	socklen_t addrLen;
  int acceptCode = 0;

	portNumber = checkArgs(argc, argv);
  serverSocket = tcpServerSetup(portNumber);
	setupPollSet();
	addToPollSet(serverSocket);
	// Recieves multiple clients
  while(1)
	{
	 // wait for client to connect
	  //clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
		 if ((clientSocket = pollCall(0)) ==  -1)
		 {
		         // exit(-1);
		 }
		 else if(clientSocket == serverSocket)
		 {
		   if ((acceptCode = accept(clientSocket, &socketAddr, &addrLen)) < 0)
		   {
				 perror("Accept call");
		     return -1;
		   }

		   addToPollSet(acceptCode);
		}
		else
		{
			printf("Client: %d connected\n", clientSocket);
	    if((code = myRecieve(clientSocket)) < 0)
	    {
				removeFromPollSet(clientSocket);
			  /* close the sockets */
			  close(clientSocket);
            //serverSocket = tcpServerSetup(portNumber);
	    }
		}
  }

	close(serverSocket);


	return 0;
}

void recvFromClient(int clientSocket)
{
	char buf[MAXBUF];
	int messageLen = 0;

	//now get the data from the client_socket
	if ((messageLen = recv(clientSocket, buf, MAXBUF, 0)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	printf("Message received, length: %d Data: %s\n", messageLen, buf);
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}

	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}

	return portNumber;
}
