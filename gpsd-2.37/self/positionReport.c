#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "gpsTest.h"
#include "arrQueue.h"
#include "position.h"
#include "dllist.h"

extern gpsSourceData gpsSource;

DLLIST *reportList = NULL;

int WalkReport(int Tag, void *p, void *Parms)
{
    struct gps_fix_t *Arg = p;
    
    printf("in report = %f\r\n", Arg->latitude);
    
  return 0;
}

void FillReportAddToList(struct gps_fix_t* gpsSrouce)
{
    positionReport_t report;
    
}

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
		                   
        DLAppend(&reportList, 0, newest, sizeof(struct gps_fix_t));

        DLWalk(reportList, WalkReport, NULL);
        
		
    }
}
