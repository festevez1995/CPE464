#include "WindowLib.h"

void createWindow(Window *window, int size)
{
  window = (Window *)malloc(sizeof(Window));
  window->windowSize = size;
  window->upperEdge = size;
  window->lowerEdge = 0;
  window->curFrame = 0;
  window->pduList = (PDUBuff *)malloc(sizeof(PDUBuff)*size);
}

void deleteWindow(Window *window)
{
  free(window->pduList);
  free(window);
}

void slide()
{

}

void makePDU(PDUBuff pdu, uint8_t *buff, int size)
{
  pdu.pduBuff = calloc(size, sizeof(uint8_t));
  memcpy(pdu.pduBuff, buff, size);
  pdu.pduLen = size;
}

void addBuffToWindow(Window *window, uint8_t *buff, int pduLen)
{
  PDUBuff pdu;
  uint32_t seqNum = getPDUSeqNum(buff);
  int idx = 0;

  if(window->curFrame < window->upperEdge)
  {
    printf("making PDU");
    makePDU(pdu, buff, pduLen);
    idx = seqNum % window->windowSize;
    printf("%d\n", idx);
    window->pduList[idx] = pdu;
    window->curFrame++;
  }
  //memcpy(&window->pduList[idx].pduBuff, buff.pduBuff, buff.pduLen);
}

void getBuffFromWindow(Window *window, PDUBuff buff, uint32_t seqNum)
{
  int idx = seqNum % window->windowSize;
  memcpy(buff.pduBuff, window->pduList[idx].pduBuff, window->pduList[idx].pduLen);
  buff.pduLen = window->pduList[idx].pduLen;
}

/* Checks to see if window is closed.
 * If closed, return a -1 else return 0.*/
int isWindowClosed(Window *window)
{
  if(window->curFrame == window->upperEdge)
  {
    return -1;
  }
return 0;
}

uint32_t getPDUSeqNum(uint8_t *pduBuff)
{
  uint32_t seqNum = 0;
  memcpy(&seqNum, pduBuff, 4);

  return ntohl(seqNum);
}

void printWindow(Window *window)
{
  int i;
  printf("Window size is %d\n", window->windowSize);
  for(i = 0; i < window->windowSize; i++)
  {
    printf("%d sequenceNumber: %d pduSize: %d\n",
    i, getPDUSeqNum(window->pduList[i].pduBuff), window->pduList[i].pduLen);
  }
}

void printServerMetadata(Window *w)
{
  printf("Server Window - Window Size: %d, lower: %d, upper: %d, current: %d\n",
  w->windowSize, w->lowerEdge, w->upperEdge, w->curFrame);
}
