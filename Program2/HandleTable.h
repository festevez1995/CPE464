#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HandleData
{
  char *handleName;
  int socket;
} HandleData;

void createHTable();
void resizeHTable();
int isHTableFull(int idx);
void addToHTable();
int getFromHTable(char *handle);
char *getHandleHTable(int idx);
void updateHTable(int socket, char *handle);
void deleteFromHTable();
void restructureHTable(int idx);
int handleExist(char *handle);
void printTable();
int getNumOfHandle();
