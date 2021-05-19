#include "WindowLib.h"

void createWindow(Window *window, int size)
{
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

void slide(Window *window, int shift)
{
  window->lowerEdge += shift;
  window->upperEdge = window->lowerEdge + window->windowSize;
  if (window->curFrame < window->lowerEdge)
  {
    window->curFrame = window->lowerEdge;
  }
}

void addBuffToWindow(Window *window, uint8_t *buff, int pduLen)
{
  PDUBuff pdu;
  uint32_t seqNum = getPDUSeqNum(buff);
  int idx = 0;

   if(window->curFrame < window->upperEdge)
   {
    pdu.pduBuff = calloc(pduLen, sizeof(uint8_t));
    memcpy(pdu.pduBuff, buff, pduLen);
    pdu.pduLen = pduLen;
    pdu.seqNum = seqNum;
    idx = seqNum % window->windowSize;
    window->pduList[idx] = pdu;
    window->curFrame++;
   }
}

void handleSREJ(Window *window, uint32_t seqNum)
{
   int idx = seqNum % window->windowSize;
   PDUBuff pdu = window->pduList[idx];
   printf("PDU from window: seqNum: %d PDUSize %d\n", seqNum, pdu.pduLen);
}

void handleRR(Window *window, int flagRR)
{
  int i;
  int count = 0;
  printf("Processing RR\n");
  for(i = 0; i < window->windowSize; i++)
  {
    PDUBuff pdu = window->pduList[i];
    if(pdu.pduLen != 0 && pdu.seqNum < flagRR)
    {
      memset(window->pduList[i].pduBuff, 0, sizeof(window->pduList[i]));
      window->pduList[i].pduLen = 0;
      window->pduList[i].seqNum = 0;
      count++;
    }
  }
  slide(window, count);
}

/* Checks to see if window is closed.
 * If closed, return a 0 else return 1.*/
int isWindowClosed(Window *window)
{
  if(window->curFrame >= window->upperEdge)
  {
    return 0;
  }
return 1;
}

uint32_t getPDUSeqNum(uint8_t *pduBuff)
{
  uint32_t seqNum = 0;
  memcpy(&seqNum, pduBuff, 4);
  seqNum = ntohl(seqNum);

  return seqNum;
}

void printWindow(Window *window)
{
  int i;
  printf("Window size is %d\n", window->windowSize);
  for(i = 0; i < window->windowSize; i++)
  {
    PDUBuff buff = window->pduList[i];
    if(buff.pduLen == 0)
    {
      printf("\t%d not valid\n", i);
    }
    else
    {
      printf("\t%d sequenceNumber: %d pduSize: %d\n", i, buff.seqNum, buff.pduLen);
    }
  }
}

void printServerMetadata(Window *w)
{
  printf("Server Window - Window Size: %d, lower: %d, upper: %d, current: %d, window open?: %d\n",
  w->windowSize, w->lowerEdge, w->upperEdge, w->curFrame, isWindowClosed(w));
}
