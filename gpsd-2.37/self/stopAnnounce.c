#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curses.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm/types.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include "gpsTest.h"
#include "arrQueue.h"
#include "stopAnnounce.h"
#include "utils.h"
#include "manager.h"

extern gpsSourceData gpsSource;

pthread_mutex_t	actionPendMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	spotPendMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t	GPSUpdate4AnnounceMutex = PTHREAD_MUTEX_INITIALIZER;

static stopPend_t lastUpdateStop;

static int GPSUpdateSignal = 0;
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
        {VALID, 120.332080,    30.277810,  ADD,    ADD,    "stop1_in.mp3",     "stop1_out.mp3"},
        {VALID, 120.332087,    30.277817,  REDUCE, REDUCE,    "stop1_in.mp3",     "stop1_out.mp3"},
    },
    {
        1,
        "stop2",
        STOP,
        {VALID, 120.332430,    30.278160,  ADD,    ADD,    "stop2_in.mp3",     "stop2_out.mp3"},
        {VALID, 120.332437,    30.278167,  REDUCE,    REDUCE,    "stop2_in.mp3",     "stop2_out.mp3"},
    },
    {
        2,
        "stop3",
        STOP,
        {VALID, 120.332780,    30.278510,  ADD,    ADD,    "stop3_in.mp3",     "stop3_out.mp3"},
        {VALID, 120.332787,    30.278517,  REDUCE,    REDUCE,    "stop3_in.mp3",     "stop3_out.mp3"},
    },
    {
        3,
        "stop4",
        STOP,
        {VALID, 120.333130,    30.278860,  ADD,    ADD,    "stop4_in.mp3",     "stop4_out.mp3"},
        {VALID, 120.333137,    30.278867,  REDUCE,    REDUCE,    "stop4_in.mp3",     "stop4_out.mp3"},
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

void addActionToActionPend(stopPendAction_t *action)
{
    pthread_mutex_lock(&actionPendMutex);
    DLAppend(&stopPendActionList, 0, action, sizeof(stopPendAction_t));
    pthread_mutex_unlock(&actionPendMutex);
}


void checkLeaveSpot(struct gps_fix_t *current, struct gps_fix_t *prev)
{
    DLLIST *pendItem = NULL;
    spotMark_t *spot = NULL;
    busStopMark_t *stop = NULL;
    //time_t tt = time(NULL);
    int upOrDown = 0;
    //printf("in checkLeaveSpot Num of stopPendList = %d\r\n", DLCount(stopPendList));
    for(pendItem = DLGetFirst(stopPendList); pendItem != NULL; pendItem = pendItem->Next)
    {
        time_t tt = time(NULL);
        stopPend_t *pend = pendItem->Object;
        if(pend->upOrDown == UPLINE)
        {
            //printf("111\r\n");
            stop = &allBusStop[pend->stopId];
            spot = &stop->upline;
            upOrDown = UPLINE;
        }
        else
        {
            //printf("112\r\n");
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
            //printf("113\r\n");
            if(UPLINE == upOrDown)
            {
              printf("%s : now leave stop : %d stop name = %s lineDir = up\r\n",ctime(&tt),  stop->id, stop->name);
              action.mp3Name = spot->leavedMp3;
              addActionToActionPend(&action);
            }
            else
            {
              printf("%s now leave stop : %d stop name = %s lineDir = down\r\n", ctime(&tt), stop->id, stop->name);
              action.mp3Name = spot->leavedMp3;
              addActionToActionPend(&action);
            }
            //printf("1131\r\n");
            pendItem->Tag = 1;//already done the work, need to be delete
          }
    }

    pthread_mutex_lock(&spotPendMutex);
    for(pendItem = DLGetFirst(stopPendList); pendItem != NULL; )
    {
        DLLIST *nextItem = pendItem->Next;
        if(pendItem != NULL && pendItem->Tag == 1)
        {
            if(pendItem == DLGetFirst(stopPendList))
            {
                //printf("aaa\r\n");
                stopPendList = pendItem->Next;
                DLDelete(pendItem);
                pendItem = stopPendList;
                continue;
            }
            else
            {
                //printf("bbb\r\n");
                DLDelete(pendItem);
            }
        }
        pendItem = nextItem;
    }  
    //printf("out checkLeaveSpot Num of stopPendList = %d\r\n", DLCount(stopPendList));
    pthread_mutex_unlock(&spotPendMutex);
}

int getNextStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *nextStop)
{
    DLLIST *item = DLGetFirst(lineData[lineId].stopList);
    DLLIST *nextItem = NULL;
    int *stopId = 0;
    int *nextStopId = 0;
    
    if(NULL == item)
    {
        return -1;
    }
    
    for(item = DLGetFirst(lineData[lineId].stopList); item != NULL; item = item->Next)
    {
        stopId = item->Object;
        if(*stopId == curStopId)
        {
            nextItem = item->Next;
            if(nextItem == NULL)//the last stop, need turn around
            {
                nextStopId = item->Object;
                nextStop->stopId = *nextStopId;
                nextStop->upOrDown = 0 - curUpOrDown;
                return nextStop;
            }
            else
            {
                nextStopId = nextItem->Object;
                nextStop->stopId = *nextStopId;
                nextStop->upOrDown = curUpOrDown;
                return nextStop;
            }
        }
    } 
    
}

int getPrevStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *prevStop)
{
    DLLIST *item = DLGetFirst(lineData[lineId].stopList);
    DLLIST *nextItem = NULL;
    int *stopId = 0;
    int *prevStopId = 0;
    
    if(NULL == item)
    {
        return -1;
    }
    
    for(item = DLGetLast(lineData[lineId].stopList); item != NULL; item = item->Prev)
    {
        stopId = item->Object;
        if(*stopId == curStopId)
        {
            nextItem = item->Prev;
            if(nextItem == NULL)//the last stop, need turn around
            {
                prevStopId = item->Object;
                prevStop->stopId = *prevStopId;
                prevStop->upOrDown = 0 - curUpOrDown;
                return prevStop;
            }
            else
            {
                prevStopId = nextItem->Object;
                prevStop->stopId = *prevStopId;
                prevStop->upOrDown = curUpOrDown;
                return prevStop;
            }
        }
    } 
    
}

void updatelastUpdateStop(int stopId, int upOrDown)
{
    lastUpdateStop.stopId = stopId;
    lastUpdateStop.upOrDown = upOrDown;
}

void enterSpot(stopPend_t *stopPend, int manuallyHandle)
{
    stopPendAction_t action;   
    busStopMark_t *stop = &allBusStop[stopPend->stopId];    
    spotMark_t *spot = NULL;
    
    if(UPLINE == stopPend->upOrDown)
    {
        spot = &stop->upline;
        action.mp3Name = spot->arrivedMp3;
    }
    else
    {
        spot = &stop->downline;
    }
    action.mp3Name = spot->arrivedMp3;
    
    if(1 != manuallyHandle)
    {
        pthread_mutex_lock(&spotPendMutex);
        DLAppend(&stopPendList, 0, stopPend, sizeof(stopPend_t));
        pthread_mutex_unlock(&spotPendMutex);
    }
    
    if(strlen(action.mp3Name) != 0)
    {
        addActionToActionPend(&action);
    }
    
    updatelastUpdateStop(stopPend->stopId, stopPend->upOrDown);
}



void updateStopJudgeList(struct gps_fix_t *current, struct gps_fix_t *prev)
{
    time_t tt = time(NULL);
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
       
        if( VALID == up->valid
            && (judgeRadius >= get_distance(up->lat, up->lng, current->latitude, current->longitude)) 
            && (judgeRadius < get_distance(up->lat, up->lng, prev->latitude, prev->longitude))
          )//entry up spot
        {
            if(1 == judgeTrendToSpot(current, prev, up->lngAttr, up->latAttr))
            {
                //stopPendAction_t action;
                stopPend.stopId = (*stopId);
                stopPend.upOrDown = UPLINE;
                //action.mp3Name = up->arrivedMp3;
                //stopPend.action = ARRIVE;
                //stopPend.action = &action;
                
                enterSpot(&stopPend, 0);
                
                /*
                pthread_mutex_lock(&spotPendMutex);
                DLAppend(&stopPendList, 0, &stopPend, sizeof(stopPend_t));
                pthread_mutex_unlock(&spotPendMutex);

                action.mp3Name = up->arrivedMp3;
                addActionToActionPend(&action);
                */
                printf("%s : now entry into stop = %d stop name = %s line dir = up \r\n",ctime(&tt), stop->id, stop->name);
            }
        }
        if( VALID == down->valid
            && (judgeRadius >= get_distance(down->lat, down->lng, current->latitude, current->longitude))
            && (judgeRadius < get_distance(down->lat, down->lng, prev->latitude, prev->longitude))
          )//entry down spot
        {
            if(1 == judgeTrendToSpot(current, prev, down->lngAttr, down->latAttr))
            {
                //stopPendAction_t action;
                stopPend.stopId = (*stopId);
                stopPend.upOrDown = DOWNLINE;
                //action.mp3Name = down->arrivedMp3;
                //stopPend.action = &action;
                
                enterSpot(&stopPend, 0);
                /*
                pthread_mutex_lock(&spotPendMutex);
                DLAppend(&stopPendList, 0, &stopPend, sizeof(stopPend_t));
                pthread_mutex_unlock(&spotPendMutex);

                action.mp3Name = down->arrivedMp3;
                addActionToActionPend(&action);
                */
                printf("%s : now entry into stop = %d stop name = %s line dir = down \r\n", ctime(&tt), stop->id, stop->name);
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
        time_t tt = time(NULL);
        char command[128];
        memset(command, 0, 128);
        
        stopPendAction_t *action = mediaItem->Object;
        sprintf(command, "madplay /opt/github_cross/gpsTest/gpsd-2.37/self/media/%s", action->mp3Name);
        printf("%s : play media name = %s\r\n", ctime(&tt), command);
        system(command);
        //sleep(3);//block for finish play
        
        mediaItem->Tag = 1;
      }
    }
    pthread_mutex_lock(&actionPendMutex);
    
    
    for(mediaItem = DLGetFirst(stopPendActionList); mediaItem != NULL; )
    {
        DLLIST *nextItem = mediaItem->Next;
        if(mediaItem != NULL && mediaItem->Tag == 1)
        {
            if(mediaItem == DLGetFirst(stopPendActionList))
            {
                //printf("aaa\r\n");
                stopPendActionList = mediaItem->Next;
                DLDelete(mediaItem);
                mediaItem = stopPendActionList;
                continue;
            }
            else
            {
                //printf("bbb\r\n");
                DLDelete(mediaItem);
            }
        }
        mediaItem = nextItem;
    } 
    pthread_mutex_unlock(&actionPendMutex);

    usleep(100000);
  }
}

void *announceGetGPSDataUpdate(void *arg)
{
	
    pthread_mutex_lock(&GPSUpdate4AnnounceMutex);
	//printf("announce get sig of gps data update\r\n");
    GPSUpdateSignal = 1;
    pthread_mutex_unlock(&GPSUpdate4AnnounceMutex);
}

int getStopIdOfLine(int lineId, int num)
{
    DLLIST *item = DLGetFirst(lineData[lineId].stopList);
    int *stopId = 0;
    int count = 0;
    
    if(num > DLCount(lineData[lineId].stopList))
    {
        return -1;
    }
    
    for(item = DLGetFirst(lineData[lineId].stopList); item != NULL; item = item->Next)
    {
        stopId = item->Object;
        if(count == num)
        {
            return (*stopId);
        }
        count = count + 1;
    }
    
    return -1;
}

void performCommandFromManager(int command)
{
    stopPend_t getPend;
    switch (command)
    {
        case NEXT_STOP_ANNOUNCE:
        {
            if(getPend.upOrDown == UPLINE)
            {
                if(-1 != getNextStop(lastUpdateStop.stopId, lastUpdateStop.upOrDown, 0, &getPend))
                {
                    printf("next stop id = %d\r\n", getPend.stopId);
                    enterSpot(&getPend, 1);
                }
            }
            else
            {
                if(-1 != getPrevStop(lastUpdateStop.stopId, lastUpdateStop.upOrDown, 0, &getPend))
                {
                    printf("next stop id = %d\r\n", getPend.stopId);
                    enterSpot(&getPend, 1);
                }
            }
            
        }
        break;
        case PREV_STOP_ANNOUNCE:
        {
            if(getPend.upOrDown == UPLINE)
            {
                if(-1 != getPrevStop(lastUpdateStop.stopId, lastUpdateStop.upOrDown, 0, &getPend))
                {
                    printf("Prev stop id = %d\r\n", getPend.stopId);
                    enterSpot(&getPend, 1);
                }
            }
            else
            {
                if(-1 != getNextStop(lastUpdateStop.stopId, lastUpdateStop.upOrDown, 0, &getPend))
                {
                    printf("prev stop id = %d\r\n", getPend.stopId);
                    enterSpot(&getPend, 1);
                }
            }
        }
        break;
        default:
        {
        }
        break;
    }
}

void* stopAnnounce(int lineId)
{
    struct gps_fix_t *newestPoint = NULL;
    struct gps_fix_t *prevPoint = NULL;
    int period = 100000;//0.1 sec
    //unsigned long count = 0;
    pthread_t mediaPlay_id;
    struct timeval timeout;
    static struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	fd_set set;
    int s = 0;
    int sock_opt = 1;
    
    
    initCity();
    initLine_0();
    initLine_1();
    
    lastUpdateStop.stopId = getStopIdOfLine(0, 0);
    lastUpdateStop.upOrDown = UPLINE;
    printf("init stopId = %d\r\n", lastUpdateStop.stopId);

    
    printCityAllBuslineInfo();
	  pthread_create(&mediaPlay_id, NULL, playTipMedia, NULL);
    
    registerNoticeClientList(NOTICE_ANNOUNCE, NULL, announceGetGPSDataUpdate);
    
    
    s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0){
		printf("error to get socket\n");
		return -1;
	}
	sock_opt = 1;

	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(void *)&sock_opt, sizeof(sock_opt)) == -1)
	{
			printf("error to set sock opt reuseaddr\n");
			return -1;

	}

			
	memset((char *)&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);
	serv_addr.sin_port = htons(PORT_ANNOUNCE);
	




	if (bind (s, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
	{
		printf("error to binding 9998\n");
		close(s);
		return -1;
	}


    for(;;)
	{
        newestPoint = NULL;
        prevPoint = NULL;
		FD_ZERO(&set);
		FD_SET(s,&set);
		timeout.tv_sec=0;
		timeout.tv_usec=100000;
        int recvCommand = 0;
        int n = 0;
   
       
        select(FD_SETSIZE,&set,NULL,NULL,&timeout);
        
        
        if (FD_ISSET(s, &set)) 
        {
			n=recvfrom(s, &recvCommand,sizeof(int),0,(struct sockaddr *)&cli_addr,&clilen);
            printf("recv command from manager %d\r\n", recvCommand);
            performCommandFromManager(recvCommand);
        }
		    
        
        pthread_mutex_lock(&GPSUpdate4AnnounceMutex);
        if(GPSUpdateSignal == 1)
        {        
            //printf("cccount = %ld \r\n", count);
            newestPoint = GetNewestDataFirst(&gpsSource);
            prevPoint = GetNewestDataSecond(&gpsSource);
            GPSUpdateSignal = 0;
            //if(count >= (100))//10 sec
            {
                updateStopJudgeList(newestPoint, prevPoint);
                checkLeaveSpot(newestPoint, prevPoint);
                //count = 0;
            }
        }
        pthread_mutex_unlock(&GPSUpdate4AnnounceMutex);
    }
}
