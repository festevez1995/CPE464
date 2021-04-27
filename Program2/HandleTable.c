#include "HandleTable.h"

/* Inital Size of htable*/
int hTableSize = 10;
int counter = 0;
int HTindex;
HandleData *handleTable = NULL;

/* Fucntion to allocatre memory for Htable.*/
void createHTable()
{
  int i;
  HTindex = 0;
  /* Allocate memory to the handleTabel with initial size of 10.*/
  handleTable = (HandleData *) malloc((sizeof(HandleData) * hTableSize));
  /* Check to see if we correctly allocated memory to the table.*/
  if (handleTable == NULL)
  {
    perror("Malloc call");
    exit(-1);
  }

  /* Initialise the htable to nothing.*/
  for(i = 0; i < hTableSize; i++)
  {
    handleTable[i].handleName = NULL;
    handleTable[i].socket = 0;
  }
}

/* function to resize htable if it has been full.*/
void resizeHTable()
{
  /* Resize htable by 2.*/
  int i;
  int old = hTableSize;
  hTableSize *= 2;

  /* Resize htable.*/
  handleTable = (HandleData *) realloc(handleTable,
                                        sizeof(HandleData) * hTableSize);
  /* Check to see if htable was allocated correctly.*/
  if(handleTable == NULL)
  {
    perror("Realloc call");
    exit(-1);
  }

  /* Initialise the htable to nothing.*/
  for(i = old; i < hTableSize; i++)
  {
    handleTable[i].handleName = NULL;
    handleTable[i].socket = 0;
  }
}

/* Fucntion to check if the Handle Table is full.
 * returns 0 when hTable isn't isHTableFull
 * returns 1 when hTable is full.
 */
int isHTableFull(int idx)
{
  if(idx >= hTableSize)
  {
    return 1;
  }
  return 0;
}

void addToHTable(char *handle, int socket)
{
  int i;

  if(handleTable[hTableSize - 1].handleName != NULL)
  {
    resizeHTable();
  }

  for(i = 0; i < hTableSize; i++)
  {
    if (handleTable[i].handleName == NULL)
    {
        handleTable[i].handleName = (char *)malloc(sizeof(char));
        memcpy(handleTable[i].handleName, handle, strlen(handle));
        handleTable[i].socket = socket;
        counter++;
        break;
    }
  }
}

int getNumOfHandle()
{
  return counter;
}
/* function that gets data from handle Table */
int getFromHTable(char *handle)
{
  int i;
  int socket = 0;
  for(i = 0; i < hTableSize; i++)
  {
    /* if handle all ready exists in table return -1*/
    if (handleTable[i].handleName != NULL){
      if(strcmp(handleTable[i].handleName, handle) == 0)
      {
        socket = handleTable[i].socket;
        break;
      }
    }
  }
  /* Return 0 if handle doesn't exist. */
  return socket;
}

char *getHandleHTable(int idx)
{
  return handleTable[idx].handleName;
}

void updateHTable(int socket, char *handle)
{
  int i;
  for(i = 0; i < hTableSize; i++)
  {
    printf("Table: %d\n", handleTable[i].socket);
    if(handleTable[i].socket == socket)
    {
      handleTable[i].handleName = handle;
      handleTable[i].socket = socket;
      break;
    }
  }
  printf("Table: %s\n", handleTable[i].handleName);

}
/* Deletes an object from the handle Table.*/
void deleteFromHTable(int socket)
{
  int i;
  int idx = -1;
  for(i = 0; i < hTableSize; i++)
  {
    if(handleTable[i].socket == socket)
    {
      free(handleTable[i].handleName);
      handleTable[i].socket = 0;
      idx = i;
      counter --;
      break;
    }
  }

  if (idx != -1)
  {
    restructureHTable(idx);
  }
}

void restructureHTable(int idx)
{
  int i;
  /*shift data from deleted idx.*/
  for(i = idx; i < hTableSize; i++)
  {
    handleTable[i] = handleTable[i+1];
  }
}

int handleExist(char *handle)
{
  int i;
  for(i = 0; i < hTableSize; i++)
  {
    /* if handle all ready exists in table return -1*/
    if (handleTable[i].handleName != NULL)
    {
      if(strcmp(handleTable[i].handleName, handle) == 0)
      {
        //printf("Handle in table\n");
        return 0;
      }
    }
  }
  //printf("Handle not in table\n");
  /* Return 0 if handle doesn't exist. */
  return -1;
}

void printTable()
{
  int i;
  for(i = 0; i < hTableSize; i++)
  {
    printf("Handle: %s, Socket %d\n", handleTable[i].handleName, handleTable[i].socket);
  }
}
/* Test Code.
int main(int argc, char *argv[])
{
  printf("Creating HTable\n\n");
  createHTable();

  int i;
  for(i = 0; i < 15; i ++)
  {
    addToHTable("Fernando", i);
  }

  for(i = 0; i < 15; i++)
  {
    HandleData myData = getFromHTable(i);
    printf("handleName: %s, socket: %d\n", myData.handleName, myData.socket);
  }
  printf("\n");
  deleteFromHTable(16);
  printf("\n");

  for(i = 0; i < 15; i++)
  {
    HandleData myData = getFromHTable(i);
    printf("handleName: %s, socket: %d\n", myData.handleName, myData.socket);
  }


  return 0;
}
*/
