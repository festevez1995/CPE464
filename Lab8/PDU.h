#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include "checksum.h"

#define MAX_PAYLOAD 1400
#define PDU_TEST_FLAG 10

int createPDU(uint8_t *pduBuff, uint32_t sequenceNumber, uint8_t flag, char *payload, int payloadSize );
void printPDU(uint8_t *pduBuff, int pduLen);
