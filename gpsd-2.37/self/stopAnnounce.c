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



const double judgeRadius = 0.003;//30m



DLLIST *stopPendList;
DLLIST *stopPendActionList;

#if 0
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

#endif

#if 0

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
#endif

int judgeTrendToSpot(struct gps_fix_t *current, struct gps_fix_t *prev, int lngAttr, int latAttr)
{
    if(lngAttr != ADD && lngAttr != REDUCE && latAttr != ADD && latAttr != REDUCE)
    {
        return -1;
    }
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
            //stop = &allBusStop[pend->stopId];
            stop = getBusStopBystopId(pend->stopId);
            spot = &stop->upline;
            upOrDown = UPLINE;
        }
        else
        {
            //printf("112\r\n");
            //stop = &allBusStop[pend->stopId];
            stop = getBusStopBystopId(pend->stopId);
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
                if(stop->type == STOP)
                {

                    FillArrivedLeaveReportAndSend(current, COMMAND_LEAVE_STOP_REPORT);
                }
            }
            else
            {
                printf("%s now leave stop : %d stop name = %s lineDir = down\r\n", ctime(&tt), stop->id, stop->name);
                action.mp3Name = spot->leavedMp3;
                addActionToActionPend(&action);
                if(stop->type == STOP)
                {
                    FillArrivedLeaveReportAndSend(current, COMMAND_LEAVE_STOP_REPORT);
                }
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



void updatelastUpdateStop(int stopId, int upOrDown)
{
    lastUpdateStop.stopId = stopId;
    lastUpdateStop.upOrDown = upOrDown;
}

void enterSpot(stopPend_t *stopPend, int manuallyHandle)
{
    stopPendAction_t action;   
    //busStopMark_t *stop = &allBusStop[stopPend->stopId];  
    busStopMark_t *stop = getBusStopBystopId(stopPend->stopId);  
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
    printf("updatelastUpdateStop stopId = %d\r\n", stopPend->stopId);
}



void updateStopJudgeList(struct gps_fix_t *current, struct gps_fix_t *prev, unsigned int lineId)
{
    time_t tt = time(NULL);
    //lineData_t *line = &lineData[0];
    lineData_t *line = getLineData(lineId);
    int upOrDown = 0;
    DLLIST *stopIdItem;
    stopPend_t stopPend;
    unsigned int i = 0;
//    printf("line id = %d line name = %s\r\n", line->lineId, line->lineName);
    for(i = 0; i < 256; i++)
    {
        int stopId = line->stopId[i];
        busStopMark_t *stop = getBusStopBystopId(stopId);
        spotMark_t *up = &stop->upline;
        spotMark_t *down = &stop->downline;

        if( VALID == up->valid
            && (judgeRadius >= get_distance(up->lat, up->lng, current->latitude, current->longitude)) 
            && (judgeRadius < get_distance(up->lat, up->lng, prev->latitude, prev->longitude))
          )//entry up spot
        {
//            printf("in stop stopId = %d\r\n", stopId);
            if(1 == judgeTrendToSpot(current, prev, up->lngAttr, up->latAttr))
            {
                stopPend.stopId = stopId;
                stopPend.upOrDown = UPLINE;
                
                enterSpot(&stopPend, 0);
                if(stop->type == STOP)
                {
                    FillArrivedLeaveReportAndSend(current, COMMAND_ARRIVED_STOP_REPORT);
                }
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
                stopPend.stopId = stopId;
                stopPend.upOrDown = DOWNLINE;
                
                enterSpot(&stopPend, 0);
                if(stop->type == STOP)
                {
                    FillArrivedLeaveReportAndSend(current, COMMAND_ARRIVED_STOP_REPORT);
                }
                printf("%s : now entry into stop = %d stop name = %s line dir = down \r\n", ctime(&tt), stop->id, stop->name);
            }
        }
    }

#if 0
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
                stopPend.stopId = (*stopId);
                stopPend.upOrDown = UPLINE;
                
                enterSpot(&stopPend, 0);

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
                stopPend.stopId = (*stopId);
                stopPend.upOrDown = DOWNLINE;
                
                enterSpot(&stopPend, 0);

                printf("%s : now entry into stop = %d stop name = %s line dir = down \r\n", ctime(&tt), stop->id, stop->name);
            }
        }
        
        
    }
#endif    
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



void performCommandFromManager(int command)
{
    stopPend_t getPend;
    switch (command)
    {
        case NEXT_STOP_ANNOUNCE:
        {
            if(getPend.upOrDown == UPLINE)
            {
                if(-1 != getNextStop(lastUpdateStop.stopId, lastUpdateStop.upOrDown, getLineId(), &getPend))
                {
                    //printf("next stop id = %d\r\n", getPend.stopId);
                    enterSpot(&getPend, 1);
                }
            }
            else
            {
                if(-1 != getPrevStop(lastUpdateStop.stopId, lastUpdateStop.upOrDown, 0, &getPend))
                {
                    //printf("next stop id = %d\r\n", getPend.stopId);
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
                    //printf("Prev stop id = %d\r\n", getPend.stopId);
                    enterSpot(&getPend, 1);
                }
            }
            else
            {
                if(-1 != getNextStop(lastUpdateStop.stopId, lastUpdateStop.upOrDown, getLineId(), &getPend))
                {
                    //printf("prev stop id = %d\r\n", getPend.stopId);
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

busStopMark_t *getStopAttr(int stopId)
{
    //return &allBusStop[stopId];
    return getBusStopBystopId(stopId);
}

spotMark_t *getSpotAttr(int stopId, int upOrDown)
{
    if(UPLINE == upOrDown)
    {
        //return &allBusStop[stopId].upline;
        return &getBusStopBystopId(stopId)->upline;
    }
    else
    {
        //return &allBusStop[stopId].downline;
        return &getBusStopBystopId(stopId)->downline;
    }
}

int getDriveDirect()
{
    return lastUpdateStop.upOrDown;
}

int getLastStopId()
{
    return lastUpdateStop.stopId;
}

int getNextStopId()
{
    return getNextStopAttr()->id;
}

busStopMark_t *getNextStopAttr()
{
    stopPend_t getPend;
//    printf("getNextStopAttr lastUpdateStop.stopId = %d\r\n", lastUpdateStop.stopId);
    if(-1 == getNextStop(lastUpdateStop.stopId, lastUpdateStop.upOrDown, getLineId(), &getPend))
    {
        return NULL;
    }
    return getStopAttr(getPend.stopId);
}

spotMark_t *getNextStopSpotAttr()
{
    stopPend_t getPend;
    if(-1 == getNextStop(lastUpdateStop.stopId, lastUpdateStop.upOrDown, getLineId(), &getPend))
    {
        return NULL;
    }
    return getSpotAttr(getPend.stopId, getPend.upOrDown);
}



unsigned short getDistance2NextStop(struct gps_fix_t *current)
{
    spotMark_t *nextSpot = getNextStopSpotAttr();
    if(NULL == nextSpot)
    {
        return 0;
    }
    return 1000*(get_distance(current->latitude, current->longitude, nextSpot->latAttr, nextSpot->lngAttr));
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
    
    
//    initCity();
//    initLine_0();
//    initLine_1();
    
    lastUpdateStop.stopId = getStopIdOfLine(getLineId(), 1);
    lastUpdateStop.upOrDown = UPLINE;
    printf("init stopId = %d\r\n", lastUpdateStop.stopId);

    
//    printCityAllBuslineInfo();
	  pthread_create(&mediaPlay_id, NULL, playTipMedia, NULL);
    
    registerNoticeClientList(NOTICE_ANNOUNCE, NULL, announceGetGPSDataUpdate);
    
    
    s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
    {
		printf("error to get socket\n");
		return ;
	}
	sock_opt = 1;

	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(void *)&sock_opt, sizeof(sock_opt)) == -1)
	{
			printf("error to set sock opt reuseaddr\n");
			return ;

	}

			
	memset((char *)&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);
	serv_addr.sin_port = htons(PORT_ANNOUNCE);
	




	if (bind (s, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
	{
		printf("error to binding 9998\n");
		close(s);
		return ;
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
                updateStopJudgeList(newestPoint, prevPoint, getLineId());
                checkLeaveSpot(newestPoint, prevPoint);
                //count = 0;
            }
        }
        pthread_mutex_unlock(&GPSUpdate4AnnounceMutex);
    }
}

void buildArrivedLeaveReportData(struct gps_fix_t *gpsData,   arrivedLeaveStopReport_t *report,   dataSendReq_t *dataSendReq, int arrivedOrLeave)
{
    unsigned int DD;
    unsigned int MM;
    unsigned int SSSS;
    unsigned short dataLen = 0;
    lineData_t *lineData = getLineData(getLineId());
    
    memset(report, 0, sizeof(arrivedLeaveStopReport_t));
    memset(dataSendReq, 0, sizeof(dataSendReq_t));
    
    getDDMMSSSS(gpsData->longitude, &DD, &MM, &SSSS);
    report->lng1 = DD;
    report->lng2 = MM;
    //report->lng3 = htons(SSSS);
    report->lng3 = (SSSS);
    
    getDDMMSSSS(gpsData->latitude, &DD, &MM, &SSSS);
    report->lat1 = DD;
    report->lat2 = MM;
    //report->lat3 = htons(SSSS);
    report->lat3 = (SSSS);

    //report->speed = htons(gpsData->speed);
    report->speed = (gpsData->speed);
    //report->azimuth = htons(0);//TODO
    report->azimuth = (0);//TODO
    report->vehicleStatus = 0;//TODO
    report->directMark = getDriveDirect();
    report->stopId = getLastStopId();
    if(NULL == lineData)
    {
        printf("invalid lineData\r\n");
        memset(report->lineId, 0, sizeof(U8)*3);
        report->lineName = "";
    }
    else
    {
        
        report->lineId[0] = lineData->lineId >> 16;
        report->lineId[1] = lineData->lineId >> 8;
        report->lineId[2] = lineData->lineId;
        report->lineName = lineData->lineName;
    }
    report->driverId = getDriverId();


    

    memcpy(dataSendReq->data + dataLen, &report->lng1, sizeof(report->lng1));
    dataLen += sizeof(report->lng1);
    memcpy(dataSendReq->data + dataLen, &report->lng2, sizeof(report->lng2));
    dataLen += sizeof(report->lng2);
    memcpy(dataSendReq->data + dataLen, &report->lng3, sizeof(report->lng3));
    dataLen += sizeof(report->lng3);
    memcpy(dataSendReq->data + dataLen, &report->lat1, sizeof(report->lat1));
    dataLen += sizeof(report->lat1);
    memcpy(dataSendReq->data + dataLen, &report->lat2, sizeof(report->lat2));
    dataLen += sizeof(report->lat2);
    memcpy(dataSendReq->data + dataLen, &report->lat3, sizeof(report->lat3));
    dataLen += sizeof(report->lat3);

    memcpy(dataSendReq->data + dataLen, &report->speed, sizeof(report->speed));
    dataLen += sizeof(report->speed);

    memcpy(dataSendReq->data + dataLen, &report->azimuth, sizeof(report->azimuth));
    dataLen += sizeof(report->azimuth);
    memcpy(dataSendReq->data + dataLen, &report->vehicleStatus, sizeof(report->vehicleStatus));
    dataLen += sizeof(report->vehicleStatus);
    memcpy(dataSendReq->data + dataLen, &report->directMark, sizeof(report->directMark));
    dataLen += sizeof(report->directMark);

    memcpy(dataSendReq->data + dataLen, &report->stopId, sizeof(report->stopId));
    dataLen += sizeof(report->stopId);

    memcpy(dataSendReq->data + dataLen, &report->lineId, sizeof(report->lineId));
    dataLen += sizeof(report->lineId);

    strcpy(dataSendReq->data + dataLen, report->lineName);
    dataLen += strlen(report->lineName)+1;

    strcpy(dataSendReq->data + dataLen, report->driverId);
    dataLen += strlen(report->driverId)+1;

    dataSendReq->commandId = arrivedOrLeave;
    dataSendReq->dataLength = dataLen;
    printf("length of arrived or leave report msg = %u\r\n", dataLen);

    return;

}


void FillArrivedLeaveReportAndSend(struct gps_fix_t* gpsData, int arriveOrLeave)
{

  arrivedLeaveStopReport_t report;
  dataSendReq_t dataSendReq;


  buildArrivedLeaveReportData(gpsData, &report, &dataSendReq, arriveOrLeave);
  dataSendReqSend(&dataSendReq);
}