#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpsTest.h"
#include "common.h"



BOOL IsQueueEmpty(gpsSourceData* queue);
void SetQueueEmpty(gpsSourceData* queue);
BOOL IsQueueFull(gpsSourceData* queue);
void EnQueue(gpsSourceData* queue, void* data, size_t sizeOfData);
void DeQueue(gpsSourceData* queue);
int GetQueueNum(gpsSourceData* queue);
void* GetNewestDataFirst(gpsSourceData* queue);




#endif