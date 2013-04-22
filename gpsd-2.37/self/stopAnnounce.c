#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "gpsTest.h"
#include "arrQueue.h"
#include "stopAnnounce.h"
#include "utils.h"

extern gpsSourceData gpsSource;

/*
typedef enum
{
    STOP = 1,
    HOTSPOT, 
    CROOKED
}stopType;

typedef enum
{
    ADD = 1, 
    REDUCE, 
    UNKNOW
}directTrend;

typedef struct
{
    double  lng;
    double  lat;
    int     lngAttr;
    int     latAttr;
    char    *arrivedMp3;
    char    *leavedMp3;
}spotMark_t;

typedef struct
{
    unsigned int id;
    char    *name;
    int type;//crooked, stop, hot
    spotMark_t  upline;
    spotMark_t  downline;    
}busStopMark_t;

typedef struct
{
    int lineId;
    char *lineName;
    DLLIST ** stopList;
}lineData_t;

typedef struct
{
    int     cityId;
    char    *cityName;
    DLLIST *lineList;
}allLineMark_t;
*/



const double judgeRadius = 0.003;//30m

lineData_t lineData[] = 
{
    {0, "yan_jiang_da_dao", NULL},
    {1, "liu_hao_lu",       NULL},
    {INVALID_ID, NULL,      NULL}
};

allLineMark_t cityData[] = 
{
    {0, "xiasha", NULL},
    {INVALID_ID, NULL, NULL}
};

busStopMark_t allBusStop[] = 
{
    {
        0,
        "stop1",
        STOP,
        {120.332080,    30.277810,  ADD,    ADD,    "up_stop1_in.mp3",     "up_stop1_out.mp3"},
        {120.332087,    30.277817,  REDUCE, REDUCE,    "down_stop1_in.mp3",     "down_stop1_out.mp3"},
    },
    {
        1,
        "stop2",
        STOP,
        {120.332430,    30.278160,  ADD,    ADD,    "up_stop2_in.mp3",     "up_stop2_out.mp3"},
        {120.332437,    30.278167,  REDUCE,    REDUCE,    "down_stop2_in.mp3",     "down_stop2_out.mp3"},
    },
    {
        2,
        "stop3",
        STOP,
        {120.332780,    30.278510,  ADD,    ADD,    "up_stop3_in.mp3",     "up_stop3_out.mp3"},
        {120.332787,    30.278517,  REDUCE,    REDUCE,    "down_stop3_in.mp3",     "down_stop3_out.mp3"},
    },
    {
        3,
        "stop4",
        STOP,
        {120.333130,    30.278860,  ADD,    ADD,    "up_stop4_in.mp3",     "up_stop4_out.mp3"},
        {120.333137,    30.278867,  REDUCE,    REDUCE,    "down_stop4_in.mp3",     "down_stop4_out.mp3"},
    },
    {
        INVALID_ID,
        NULL,
        STOP,
        {0,    0,  UNKNOW,    UNKNOW,    NULL,     NULL},
        {0,    0,  UNKNOW,    UNKNOW,    NULL,     NULL},
    },
    
};

void printCityAllBuslineInfo()
{
    int i = 0;
    int j = 0;
    allLineMark_t *city;
    for(i = 0; ;i++)
    {
        city = &cityData[i];
        DLLIST *lineIdItem;
        if(city->cityId == INVALID_ID)
        {
            break;
        }
        printf("city id = %d city name = %s\r\n", city->cityId, city->cityName);
        for(lineIdItem = city->lineList; lineIdItem != NULL; lineIdItem = lineIdItem->Next)
        {
            int *lineId = lineIdItem->Object;
            if(*lineId != INVALID_ID)
            {
                lineData_t *line = &lineData[*lineId];
                DLLIST *stopIdItem;
                printf("line id = %d line name = %s\r\n", line->lineId, line->lineName);
                for(stopIdItem = line->stopList; stopIdItem != NULL; stopIdItem = stopIdItem->Next)
                {
                    int *stopId = stopIdItem->Object;
                    
                    busStopMark_t *stop = &allBusStop[*stopId];
                    spotMark_t *up = &stop->upline;
                    spotMark_t *down = &stop->downline;
                    
                    printf("stopId = %d\r\n", *stopId);
                    
                    if((*stopId) != INVALID_ID)
                    {
                        printf("stop id = %d stop name = %s stop type = %d \r\n", stop->id, stop->name, stop->type);
                        printf("up: lng = %f lat = %f lngAttr = %d latAttr = %d arrivedMp3 = %s leavedMp3 = %s\r\n", 
                            up->lng, up->lat, up->lngAttr, up->latAttr, up->arrivedMp3, up->leavedMp3);
                        printf("down: lng = %f lat = %f lngAttr = %d latAttr = %d arrivedMp3 = %s leavedMp3 = %s\r\n", 
                            down->lng, down->lat, down->lngAttr, down->latAttr, down->arrivedMp3, down->leavedMp3);
                    }
                    
                }
            }
        }
    }
}
/*
void printCityAllBus()
{
    DLLIST *lineItem;
    for(lineItem = xiasha.lineList; lineItem != NULL; lineItem = lineItem->Next)
    {
        lineData_t * lineData = (lineData_t *)lineItem->Object;
        printf(" line id = %d lineName = %s\r\n", lineData->lineId, lineData->lineName);
        printf("have stops -----------------------------------\r\n");
        if(INVALID_ID != lineData->lineId)
        {
            DLLIST *stopItem;
            for(stopItem = lineData->stopList; stopItem != NULL; stopItem = stopItem->Next)
            {
                busStopMark_t *stopData = (busStopMark_t *)stopItem->Object;
                {
                    
                    spotMark_t *up = &stopData->upline;
                    spotMark_t *down = &stopData->downline;
                    printf("stop id = %d stop name = %s stop type = %d \r\n", stopData->id, stopData->name, stopData->type);
                    printf("up: lng = %f lat = %f lngAttr = %d latAttr = %d arrivedMp3 = %s leavedMp3 = %s\r\n", 
                        up->lng, up->lat, up->lngAttr, up->latAttr, up->arrivedMp3, up->leavedMp3);
                    printf("down: lng = %f lat = %f lngAttr = %d latAttr = %d arrivedMp3 = %s leavedMp3 = %s\r\n", 
                        down->lng, down->lat, down->lngAttr, down->latAttr, down->arrivedMp3, down->leavedMp3);
                }
            }
        }
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\r\n");
    }
}
*/
/*
void initLine1()
{
    line1.lineId = 0;
    line1.lineName = "yan_jiang_da_dao";
    DLAppend(&line1.stopList, 0, &allBusStop[0], sizeof(busStopMark_t));
    DLAppend(&line1.stopList, 0, &allBusStop[1], sizeof(busStopMark_t));
    DLAppend(&line1.stopList, 0, &allBusStop[2], sizeof(busStopMark_t));
    DLAppend(&line1.stopList, 0, &allBusStop[3], sizeof(busStopMark_t));
}
*/


void initLine_0()
{
    
    int stop0 = 0;
    int stop1 = 1;
    int stop2 = 2;
    int stop3 = 3;
    //int stopEnd = -1;
    
    DLAppend(&lineData[0].stopList, 0, &stop0, sizeof(int));
    DLAppend(&lineData[0].stopList, 0, &stop1, sizeof(int));
    DLAppend(&lineData[0].stopList, 0, &stop2, sizeof(int));
    DLAppend(&lineData[0].stopList, 0, &stop3, sizeof(int));
    //DLAppend(&lineData[0].stopList, 0, &stopEnd, sizeof(int));
}

void initLine_1()
{
}

void initCity()
{
    int line0 = 0;
    int line1 = 1;
    DLAppend(&cityData[0].lineList, 0, &line0, sizeof(int));//add line 0 to city
    DLAppend(&cityData[0].lineList, 0, &line1, sizeof(int));//add line 1 to city
    
}

void* stopAnnounce()
{
    struct gps_fix_t *newest = NULL;
    struct gps_fix_t *second = NULL;
    
    initCity();
    initLine_0();
    initLine_1();

    
    printCityAllBuslineInfo();
    
    for(;;)
    {
        sleep(2);
        
        printf("stopAnnounce \r\n");
        
        newest = GetNewestDataFirst(&gpsSource);
        second = GetNewestDataSecond(&gpsSource);
        
        printf("tail latitude = %f\r\n", newest->latitude);
        printf("tail latitude = %f\r\n", second->latitude);
    }
}
