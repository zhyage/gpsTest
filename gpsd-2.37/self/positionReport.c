#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "gpsTest.h"
#include "arrQueue.h"
#include "position.h"

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
		
		printf("sizeof int = %d\r\n", sizeof(int));
		printf("sizeof short = %d\r\n", sizeof(short));
		printf("sizeof long = %d\r\n", sizeof(long));
		
    }
}
