#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "gpsTest.h"
#include "arrQueue.h"
#include "stopAnnounce.h"
#include "utils.h"

extern gpsSourceData gpsSource;

pthread_mutex_t	actionPendMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	spotPendMutex = PTHREAD_MUTEX_INITIALIZER;
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
        {VALID, 120.332080,    30.277810,  ADD,    ADD,    "up_stop1_in.mp3",     "up_stop1_out.mp3"},
        {VALID, 120.332087,    30.277817,  REDUCE, REDUCE,    "down_stop1_in.mp3",     "down_stop1_out.mp3"},
    },
    {
        1,
        "stop2",
        STOP,
        {VALID, 120.332430,    30.278160,  ADD,    ADD,    "up_stop2_in.mp3",     "up_stop2_out.mp3"},
        {VALID, 120.332437,    30.278167,  REDUCE,    REDUCE,    "down_stop2_in.mp3",     "down_stop2_out.mp3"},
    },
    {
        2,
        "stop3",
        STOP,
        {VALID, 120.332780,    30.278510,  ADD,    ADD,    "up_stop3_in.mp3",     "up_stop3_out.mp3"},
        {VALID, 120.332787,    30.278517,  REDUCE,    REDUCE,    "down_stop3_in.mp3",     "down_stop3_out.mp3"},
    },
    {
        3,
        "stop4",
        STOP,
        {VALID, 120.333130,    30.278860,  ADD,    ADD,    "up_stop4_in.mp3",     "up_stop4_out.mp3"},
        {VALID, 120.333137,    30.278867,  REDUCE,    REDUCE,    "down_stop4_in.mp3",     "down_stop4_out.mp3"},
    },
    {
        INVALID_ID,
        NULL,
        STOP,
        {INVALID, 0,    0,  UNKNOW,    UNKNOW,    NULL,     NULL},
        {INVALID, 0,    0,  UNKNOW,    UNKNOW,    NULL,     NULL},
    },
    
};

DLLIST *stopPendList;
DLLIST *stopPendActionList;

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

int judgeTrendToSpot(struct gps_fix_t *current, struct gps_fix_t *prev, int lngAttr, int latAttr)
{
    if(lngAttr == ADD)
    {
        if(current->longitude - prev->longitude < 0)
        {
            return -1;
        }
    }
    else if(lngAttr == REDUCE)
    {
        if(current->longitude - prev->longitude > 0)
        {
            return -1;
        }
    }
    if(latAttr == ADD)
    {
        if(current->latitude - prev->latitude < 0)
        {
            return -1;
        }
    }
    else if(latAttr == REDUCE)
    {
        if(current->latitude - prev->latitude > 0)
        {
            return -1;
        }
    }
    return 1;
    
}

void checkLeaveSpot(struct gps_fix_t *current, struct gps_fix_t *prev)
{
    DLLIST *pendItem = NULL;
    spotMark_t *spot = NULL;
    busStopMark_t *stop = NULL;
    int upOrDown = 0;
    printf("in checkLeaveSpot Num of stopPendList = %d\r\n", DLCount(stopPendList));
    for(pendItem = DLGetFirst(stopPendList); pendItem != NULL; pendItem = pendItem->Next)
    {
        stopPend_t *pend = pendItem->Object;
        if(pend->upOrDown == UPLINE)
        {
            printf("111\r\n");
            stop = &allBusStop[pend->stopId];
            spot = &stop->upline;
            upOrDown = UPLINE;
        }
        else
        {
            printf("112\r\n");
            stop = &allBusStop[pend->stopId];
            spot = &stop->downline;
            upOrDown = DOWNLINE;
        }
        if(
            (judgeRadius < get_distance(spot->lat, spot->lng, current->latitude, current->longitude)) 
            && (judgeRadius >= get_distance(spot->lat, spot->lng, prev->latitude, prev->longitude))
          )
          {
            stopPendAction_t action;
            printf("113\r\n");
            if(UPLINE == upOrDown)
            {
              printf("now leave stop : %d stop name = %s lineDir = up\r\n", stop->id, stop->name);
              action.mp3Name = spot->leavedMp3;
              
              pthread_mutex_lock(&actionPendMutex);
              DLAppend(&stopPendActionList, 0, &action, sizeof(stopPendAction_t));
              pthread_mutex_unlock(&actionPendMutex);
            }
            else
            {
              printf("now leave stop : %d stop name = %s lineDir = down\r\n", stop->id, stop->name);
              action.mp3Name = spot->leavedMp3;
              pthread_mutex_lock(&actionPendMutex);
              DLAppend(&stopPendActionList, 0, &action, sizeof(stopPendAction_t));
              pthread_mutex_unlock(&actionPendMutex);
            }
            printf("1131\r\n");
            pendItem->Tag = 1;//already done the work, need to be delete
/*
            if(NULL == pendItem->Next && NULL == pendItem->Prev)
            {
                stopPendList = NULL;
            }
            DLDelete(pendItem);
*/
          }
    }

    pthread_mutex_lock(&spotPendMutex);
    for(pendItem = DLGetFirst(stopPendList); pendItem != NULL; pendItem = pendItem->Next)
    {
      if(pendItem->Tag == 1)//delete
      {
        DLDelete(pendItem);
      }
    }
    /* issue in dllist, the last one can't be delete from list work around*/
    if(DLGetFirst(stopPendList) == DLGetLast(stopPendList))
    {
      printf("114\r\n");
      pendItem = DLGetFirst(stopPendList);
      if(pendItem != NULL && pendItem->Tag == 1)
      {
        printf("115\r\n");
        DLDestroy(&stopPendList);
      }
    }
    pthread_mutex_unlock(&spotPendMutex);
}

void updateStopJudgeList(struct gps_fix_t *current, struct gps_fix_t *prev)
{
    lineData_t *line = &lineData[0];
    int upOrDown = 0;
    DLLIST *stopIdItem;
    stopPend_t stopPend;
    //printf("line id = %d line name = %s\r\n", line->lineId, line->lineName);
    for(stopIdItem = line->stopList; stopIdItem != NULL; stopIdItem = stopIdItem->Next)
    {
        int *stopId = stopIdItem->Object;
        
        busStopMark_t *stop = &allBusStop[*stopId];
        spotMark_t *up = &stop->upline;
        spotMark_t *down = &stop->downline;
/*        
        if(*stopId == 1)
        {
            printf("distance with stop 1 = %f\r\n", get_distance(up->lat, up->lng, current->latitude, current->longitude));
        }
*/        
        if( VALID == up->valid
            && (judgeRadius >= get_distance(up->lat, up->lng, current->latitude, current->longitude)) 
            && (judgeRadius < get_distance(up->lat, up->lng, prev->latitude, prev->longitude))
          )//entry up spot
        {
            if(1 == judgeTrendToSpot(current, prev, up->lngAttr, up->latAttr))
            {
                stopPendAction_t action;
                stopPend.stopId = (*stopId);
                stopPend.upOrDown = UPLINE;
                stopPend.action = ARRIVE;
                pthread_mutex_lock(&spotPendMutex);
                DLAppend(&stopPendList, 0, &stopPend, sizeof(stopPend_t));
                pthread_mutex_unlock(&spotPendMutex);

                action.mp3Name = up->arrivedMp3;
                pthread_mutex_lock(&actionPendMutex);
                DLAppend(&stopPendActionList, 0, &action, sizeof(stopPendAction_t));
                pthread_mutex_unlock(&actionPendMutex);
                printf("now entry into stop = %d stop name = %s line dir = up \r\n", stop->id, stop->name);
            }
        }
        if( VALID == down->valid
            && (judgeRadius >= get_distance(down->lat, down->lng, current->latitude, current->longitude))
            && (judgeRadius < get_distance(up->lat, up->lng, prev->latitude, prev->longitude))
          )//entry down spot
        {
            if(1 == judgeTrendToSpot(current, prev, down->lngAttr, down->latAttr))
            {
                stopPendAction_t action;
                stopPend.stopId = (*stopId);
                stopPend.upOrDown = DOWNLINE;
                stopPend.action = ARRIVE;
                pthread_mutex_lock(&spotPendMutex);
                DLAppend(&stopPendList, 0, &stopPend, sizeof(stopPend_t));
                pthread_mutex_unlock(&spotPendMutex);

                action.mp3Name = down->arrivedMp3;
                pthread_mutex_lock(&actionPendMutex);
                DLAppend(&stopPendActionList, 0, &action, sizeof(stopPendAction_t));
                pthread_mutex_unlock(&actionPendMutex);
                printf("now entry into stop = %d stop name = %s line dir = down \r\n", stop->id, stop->name);
            }
        }
        
        
    }
}

void *playTipMedia()
{
  int count = 0;
  for(;;)
  {
    DLLIST *mediaItem;
    for(mediaItem = DLGetFirst(stopPendActionList); mediaItem != NULL; mediaItem = mediaItem->Next)
    {
      if(mediaItem->Object != NULL)
      {
        stopPendAction_t *action = mediaItem->Object;
        printf("play media name = %s\r\n", action->mp3Name);
        sleep(3);//block for finish play
        mediaItem->Tag = 1;
      }
    }
    pthread_mutex_lock(&actionPendMutex);
    for(mediaItem = DLGetFirst(stopPendActionList); mediaItem != NULL; mediaItem = mediaItem->Next)
    {
      if(mediaItem->Tag == 1)
      {
        DLDelete(mediaItem);
      }
    }
    /* work around dllist issue */
    if(DLGetFirst(stopPendActionList) == DLGetLast(stopPendActionList))
    {
      mediaItem = DLGetFirst(stopPendActionList);
      if(mediaItem != NULL && mediaItem->Tag == 1)
      {
        DLDestroy(&stopPendActionList);
      }
    }
    pthread_mutex_unlock(&actionPendMutex);

    sleep(1);
  }
}

void* stopAnnounce()
{
    struct gps_fix_t *newest = NULL;
    struct gps_fix_t *second = NULL;
    pthread_t mediaPlay_id;
    
    initCity();
    initLine_0();
    initLine_1();

    
    printCityAllBuslineInfo();
	  pthread_create(&mediaPlay_id, NULL, playTipMedia, NULL);
    
    for(;;)
    {
        sleep(1);
        
        //printf("stopAnnounce \r\n");
        
        newest = GetNewestDataFirst(&gpsSource);
        second = GetNewestDataSecond(&gpsSource);
        
        //printf("tail latitude = %f\r\n", newest->latitude);
        //printf("tail latitude = %f\r\n", second->latitude);
        printf("111111111111111111111111111111111111\r\n");
        updateStopJudgeList(newest, second);
        checkLeaveSpot(newest, second);
        printf("222222222222222222222222222222222222\r\n");
    }
}
