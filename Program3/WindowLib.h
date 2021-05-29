#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

typedef struct PDUBuff
{
  uint8_t *pduBuff;
  int pduLen;
  uint32_t seqNum;
} PDUBuff;

typedef struct Window
{
  int windowSize;
  int lowerEdge;
  int upperEdge;
  int curFrame;
  PDUBuff *pduList;
} Window;

void createWindow(Window *window, int windowSize, int buffSize);
void deleteWindow(Window *window);
void slide();
void addBuffToWindow(Window *window, uint8_t *buff, int pduLen);
void handleSREJ(Window *window, uint32_t seqNum);
void handleRR(Window *window, int flagRR);
int isWindowClosed(Window *window);
uint32_t getPDUSeqNum(uint8_t *pduBuff);
void printWindow(Window *window);
void printServerMetadata(Window *w);
