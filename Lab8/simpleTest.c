#include <stdio.h>
#include <string.h>
#include "PDU.h"
#include "WindowLib.h"

#define FLAG 0
#define MAX_SIZE 1407

void testWindowLib();
int getRandomString(char *data);

int main()
{
  testWindowLib();

  return 0;
}

void testWindowLib()
{
  int windowSize = 4;
  Window window;
  int seqNum = 0;

  createWindow(&window, windowSize);


  while(1)
  {
    /* Window is Open.*/
    if(isWindowClosed(&window) == 1)
    {
      /* Create PDU*/
      uint8_t buff[MAX_SIZE];
      char *data = "";
      int datalen = getRandomString(data);
      int pduLen = createPDU(buff, seqNum++, FLAG, data, datalen);
      /* Add PDU to the table.*/
      addBuffToWindow(&window, buff, pduLen);
      printServerMetadata(&window);
    }
    /*Window is closed.*/
    else
    {
      char valRR[10];
      printWindow(&window);
      printServerMetadata(&window);
      printf("\nEnter number to RR: ");
      scanf("%s", valRR);
      handleRR(&window, atoi(valRR));
      printWindow(&window);
      printServerMetadata(&window);
    }
  }

}

int getRandomString(char *data)
{
  int x = rand() % 4;
  int len = 0;
  switch (x) {
    case 0:
      data = "This is data 1.";
      len = 16;
      break;
    case 1:
      data = "Hello";
      len = 6;
      break;
    case 2:
      data = "Hi";
      len = 3;
      break;
    case 3:
      data = "This is data 4.";
      len = 16;
      break;
  }
  return len;
}
