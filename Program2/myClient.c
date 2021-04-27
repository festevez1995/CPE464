/******************************************************************************
* myClient.c
*
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
#include "ClientControl.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

void sendToServer(int socketNum);
//int readFromStdin(char * buffer);
int checkHandle(char *handle);
void checkArgs(int argc, char * argv[]);

int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor

	checkArgs(argc, argv);

	/* Check for handle length or if handle starts with a number*/
	if(checkHandle(argv[1]) < 0)
	{
		fflush(stdout);
		return 0;
	}

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);
	/* Loop through this until exit is pressed.*/
  processClientSocket(argv[1], socketNum);

	return 0;
}

int checkHandle(char *handle)
{
  if(strlen(handle) > 100)
  {
    printf("Invalid handle, handle longer than 100 characters: %s\n", handle);
    return -1;
  }
  else if( handle[0] == '0' || handle[0] == '1' || handle[0] == '2' ||
           handle[0] == '3' || handle[0] == '4' || handle[0] == '5' ||
           handle[0] == '6' || handle[0] == '7' || handle[0] == '8' ||
           handle[0] == '9')
  {
    printf("Invalid handle, handle starts with a number\n");
    return -1;
  }
  return 0;
}

void sendToServer(int socketNum)
{
	char sendBuf[MAXBUF];   //data buffer
	int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */

	sendLen = readFromStdin(sendBuf);
	printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);

	sent =  send(socketNum, sendBuf, sendLen, 0);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}

	printf("Amount of data sent is: %d\n", sent);
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s handle host-name port-number \n", argv[0]);
		exit(1);
	}
}
