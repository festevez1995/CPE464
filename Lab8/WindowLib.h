#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PDUBuff
{
  uint8_t *pduBuff;
  int pduLen;

} PDUBuff;

typedef struct Window
{
  int windowSize;
  int lowerEdge;
  int upperEdge;
  int curFrame;
  PDUBuff *pduList;
} Window;

void createWindow(Window *window, int size);
void deleteWindow(Window *window);
void slide();
void makePDU(PDUBuff pdu, uint8_t *buff, int size);
void addBuffToWindow(Window *window, uint8_t *buff, int pduLen);
void getBuffFromWindow(Window *window, PDUBuff buff, uint32_t seqNum);
int isWindowClosed(Window *window);
uint32_t getPDUSeqNum(uint8_t *pduBuff);
void printWindow(Window *window);
void printServerMetadata(Window *w);
