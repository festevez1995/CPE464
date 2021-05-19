#include <stdio.h>
#include <string.h>
#include "PDU.h"
#include "WindowLib.h"

int main()
{
  int windowSize = 4;
  Window *window = NULL;
  uint8_t buff[1407];
  int i;

  char *data1 = "This is data 1.";
  char *data2 = "Hello";
  char *data3 = "Hi";
  char *data4 = "This is data 4.";

  createWindow(window, windowSize);

  for(i = 0; i < 4; i++)
  {
    int size1 = 0;
    switch (i)
    {
      case 0:
        size1 = createPDU(buff, i, 10, data1, 16);
        break;
      case 1:
        size1 = createPDU(buff, i, 10, data2, 6);
        break;
      case 2:
        size1 = createPDU(buff, i, 10, data3, 3);
        break;
      case 3:
        size1 = createPDU(buff, i, 10, data4, 16);
        break;
    }

    addBuffToWindow(window, buff, size1);
    memset(buff, 0, 1407);
  }

  //printWindow(window);
  //ßprintServerMetadata(window);

  return 0;
}
