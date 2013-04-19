#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "gpsTest.h"
#include "queue.h"

extern gpsSourceData gpsSource;

void* positionReport()
{
    struct gps_fix_t *newest = NULL;
    struct gps_fix_t *second = NULL;
    
    for(;;)
    {
        sleep(5);
        
        printf("positionReport \r\n");
        
        newest = GetNewestDataFirst(&gpsSource);
        second = GetNewestDataSecond(&gpsSource);
        
        printf("tail latitude = %f\r\n", newest->latitude);
        printf("tail latitude = %f\r\n", second->latitude);
    }
}