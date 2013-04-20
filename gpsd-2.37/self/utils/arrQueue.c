#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpsTest.h"



BOOL IsQueueEmpty(gpsSourceData* queue)
{
    if(queue->head == queue->tail)        
    {
        return TRUE;
    }
    return FALSE;
}


void SetQueueEmpty(gpsSourceData* queue)
{
    queue->tail = 0;        
    queue->head = 0;
    return;
}


BOOL IsQueueFull(gpsSourceData* queue)
{

    if((queue->tail + 1) % MISC_BUF_SIZE == queue->head)
    {
        return TRUE;
    }
    return FALSE;

}


void EnQueue(gpsSourceData* queue, void* data, size_t sizeOfData)
{
    if(IsQueueFull(queue))
    {
        DeQueue(queue);
    }
    //queue->data[queue->tail] = data;
    memcpy(&queue->data[queue->tail], data, sizeOfData);
    queue->tail = (queue->tail + 1) % MISC_BUF_SIZE;
}


void DeQueue(gpsSourceData* queue)
{
    if(IsQueueEmpty(queue))
    {
        return;
    }
    queue->head = (queue->head + 1) % MISC_BUF_SIZE;
    return;
}

int GetQueueNum(gpsSourceData* queue)
{
    int n = queue->tail - queue->head;
    if(IsQueueEmpty(queue))
    {
        return 0;
    }
    if(n >= 0)
    {
        return n + 1;
    }
    else
    {
        return (MISC_BUF_SIZE + 1 + n);
    }
}

void* GetNewestDataFirst(gpsSourceData* queue)
{
    if(queue->tail == 0)
    {
        return &queue->data[MISC_BUF_SIZE - 1];
    }
    else
    {
        return &queue->data[(queue->tail)-1];
    }

}

void* GetNewestDataSecond(gpsSourceData* queue)
{
    if(queue->tail == 0)
    {
        return &queue->data[MISC_BUF_SIZE - 2];
    }
    if(queue->tail - 1 == 0)
    {
        return &queue->data[MISC_BUF_SIZE - 1];
    }
    else
    {
        return &queue->data[(queue->tail -1 ) -1 ];
    }
}
