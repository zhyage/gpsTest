#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/dllist.h"
//aaa
DLLIST *tryList = NULL;

printList()
{
  DLLIST *item = NULL;
  for(item = DLGetFirst(tryList); item != NULL; item = item->Next)
  {
    int *i = item->Object;
    if(NULL == i)
    {
        printf("NULL in list\r\n");
    }
    else
    {
        printf(" %d ", (*i));
    }
  }
  
  printf("total count = %d\r\n", DLCount(tryList));
}

int DelItem(DLLIST *item)
{
    if(DLGetFirst(tryList) == item)//delete first item
    {
        tryList = item->Next;
        DLDelete(item);
        return 1;
    }
    else
    {
        DLDelete(item);
        return 0;
    }
}

int main()
{
  int i = 0;
  int j = 0;
  DLLIST *item = NULL;
  for(i = 1; i <= 10; i++)
  {
    DLAppend(&tryList, 0, &i, sizeof(int));
  }
  printList();

  for(item = DLGetFirst(tryList); item != NULL; item = item->Next)
  {
    j = j + 1;
    if(j == 1 || j == 6 || j == 10)
    {
        item->Tag = 0;
    }
    else
    {
        item->Tag = 1;
    }
    printf("totalaa count = %d\r\n", DLCount(tryList));
  }
  printf("j = %d\r\n", j);

  if(tryList != NULL)
  {
    for(item = DLGetFirst(tryList); item != NULL;)
    {
       DLLIST *nextItem = item->Next;
        if(item != NULL && item->Tag == 1)
        {
            if(item == DLGetFirst(tryList))
            {
                printf("aaa\r\n");
                tryList = item->Next;
                DLDelete(item);
                item = tryList;
                continue;
            }
            else
            {
                printf("bbb\r\n");
                DLDelete(item);
                //item = nextItem;
            }
        }
        item = nextItem;
    }
    
  }
  printList();


}
