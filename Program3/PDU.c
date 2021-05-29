#include "PDU.h"

/* pduBuff = Buffer you fill
 * sequenceNumber = 32 bit sequence number in network order
 * flag = the type of pdu flag (flag 3 means data)
 * payload = payload(data) of the PDU(may not be null terminated)
 * payloadSize = the legnth of the payload
 *
 * returns the lenght of the created PDU
 */
int createPDU(uint8_t *pduBuff, uint32_t sequenceNumber, uint8_t flag, char *payload, int payloadSize )
{
  uint32_t seqNum = htonl(sequenceNumber);
  uint16_t checksum = 0;
  int offset = 0;

  /* Add seq number to the begining of the PDU buff.*/
  //((uint32_t *)pduBuff)[offset] = seqNum;
  memcpy(pduBuff, &seqNum, 4);
  /* Offset is change to 4 because of the 4 bytes */
  offset += 4;
  /*Check sum bytes. Do nothing.*/
  ((uint16_t *)pduBuff)[offset] = 0;
  //memcpy(&pduBuff[offset], 0, 2);
  offset +=2;
  /* Add flag to the pduBuff.*/
  pduBuff[offset] = flag;
  /* Increment the offset by 1 byte.*/
  offset += 1;
  /* add Payload data to the pdu buff.*/
  memcpy(&pduBuff[offset], payload, payloadSize);
  /* Add the check sum to the pduBuff.*/
  checksum = in_cksum((unsigned short *)pduBuff, offset + payloadSize);
  memcpy(&pduBuff[4], &checksum, 2);


return offset + payloadSize;
}

void printPDU(uint8_t *pduBuff, int pduLen)
{
  uint32_t seqNum = 0;
  uint16_t checksum = 0;
  uint8_t flag = 0;
  uint8_t payload[MAX_PAYLOAD];
  int offset = 0;
  /* copy the sequence number from pdu buff*/
  memcpy(&seqNum, pduBuff, 4);
  offset += 4;
  /* Copy the checksum value.*/
  memcpy(&checksum, &pduBuff[offset], 2);
  offset += 2;
  /* Copy the flag value.*/
  memcpy(&flag, &pduBuff[offset], 1);
  offset += 1;
  /*Copy Payload.*/
  memcpy(payload, &pduBuff[offset], pduLen - 7);
  /* Dispaly all the data. except for payload. */
  printf("Sequence Number: %d\n", ntohl(seqNum));
  printf("CheckSum: %d\n", checksum);
  printf("Flag: %d\n", flag);
  printf("Payload: %s\n", payload);
  printf("Payload length: %d\n", pduLen - 7);
}

void printInitPayload(char *payload)
{
  int windowSize = 0;
  int buffSize = 0;
  char filename[100];

  memcpy(&windowSize, payload, 1);
  memcpy(&buffSize, &payload[1], 4);
  memcpy(&filename, &payload[5], 100);

  printf("Window Size: %d, Buffsize: %d, Filename: %s\n", windowSize, buffSize, filename);

}
