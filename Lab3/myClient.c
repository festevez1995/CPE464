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
#include "pduSendRecv.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

void sendToServer(int socketNum);
int readFromStdin(char * buffer);
void checkArgs(int argc, char * argv[]);

int main(int argc, char * argv[])
{
	/* Buffer to hold the PDU Header block.*/
	char pdu[MAXBUF];
	/* Buffer to hold the cleint message.*/
	char sendBuf[MAXBUF];
	int socketNum = 0;         //socket descriptor
	/* Holds the message total length.*/
  uint16_t messageLen = 0;
	/* Holds the total length for the PDU length.*/
	uint16_t pduLen = 2;

	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[1], argv[2], DEBUG_FLAG);
	/* Loop through this until exit is pressed.*/
	while(strcmp(sendBuf, "exit") != 0)
	{
		/* Read the clients message and calculate the lenght of it.*/
		messageLen = readFromStdin(sendBuf);
		pduLen += messageLen; //Calculating pduLen
		/* Convert pdulen to network order.*/
		pduLen = htons(pduLen);
    /* Copy data into the pdu buffer.*/
		memcpy(pdu, &pduLen, PDU_LEN_BYTES);
		memcpy(&pdu[2], sendBuf, messageLen);

	  mySend(socketNum, pdu, messageLen + 2);

		messageLen = 0;
		pduLen = 2;
  }
	// /* On exit, send the exit message.*/
	// /* pduLen is 2 bytes pluse the size of exit including the null character.*/
	// pduLen = 2 + 5;
	// memcpy(pdu, &pduLen, PDU_LEN_BYTES);
	// memcpy(&pdu[2], "exit\0", messageLen);
	// if (send(socketNum, pdu, pduLen, 0) < 0)
	// {
	//     perror("send call");
	//     exit(-1);
	// }
 	close(socketNum);

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

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 3)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}
