#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/dllist.h"

DLLIST *tryList = NULL;

printList()
{
  DLLIST *item = NULL;
  for(item = tryList; item != NULL; item = item->Next)
  {
    int *i = item->Object;
    printf(" obj = %d ", (*i));
  }
  printf("\r\n");
}

int main()
{
  int i = 0;
  DLLIST *item = NULL;
  for(i = 0; i < 10; i++)
  {
    DLAppend(&tryList, 0, &i, sizeof(int));
  }
  printList();
/*
  for(item = tryList; item != NULL; item = item->Next)
  {
    DLDelete(item);
  }
*/
  if(tryList != NULL)
  {
    for(item = DLGetFirst(tryList); item != NULL; item = item->Next)
    {
      DLDelete(item);
    }
    if(DLGetFirst(tryList) == DLGetLast(tryList))
    {
      printf("left only 1\r\n");
    }
  }
  printList();


}
