#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include "gpsTest.h"
#include "arrQueue.h"
#include "positionReport.h"
#include "dllist.h"
#include "utils.h"
#include "sendSession.h"
#include "stopAnnounce.h"
#include "lineMap.h"
#include "identity.h"

pthread_mutex_t	positionReportMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	GPSUpdateMutex = PTHREAD_MUTEX_INITIALIZER;

extern gpsSourceData gpsSource;
FILE *posFd = NULL;

DLLIST *reportList = NULL;

static int GPSUpdateSignal = 0;

int getNumOfList()
{
  int i = 0;
  DLLIST *thisItem = reportList;
  for(thisItem = reportList; thisItem != NULL; thisItem = thisItem->Next)
  {
    i = i + 1;
  }
  return i;
}
#if 0
void *sendPositionReport(DLLIST **list)
{
  pthread_mutex_lock(&positionReportMutex);
  DLLIST *thisItem;
  for(thisItem = DLGetFirst(*list); thisItem != NULL; thisItem = thisItem->Next)
  {
    char string[256];
    time_t tt = time(NULL);
    positionReport_t *arg = thisItem->Object;
    memset(&string, 0, sizeof(string));
    sprintf(string, "time : %s longitude : %x latitude : %x\r\n", ctime(&tt), arg->longitude, arg->latitude);
    thisItem->Tag = 1;//already sent
    fputs(string, posFd);
    printf(" %s \r\n", string);
    fflush(posFd);
  }
  
    for(thisItem = DLGetFirst(*list); thisItem != NULL; )
    {
        DLLIST *nextItem = thisItem->Next;
        if(thisItem != NULL && thisItem->Tag == 1)
        {
            if(thisItem == DLGetFirst(*list))
            {
                //printf("aaa\r\n");
                *list = thisItem->Next;
                DLDelete(thisItem);
                thisItem = *list;
                continue;
            }
            else
            {
                //printf("bbb\r\n");
                DLDelete(thisItem);
            }
        }
        thisItem = nextItem;
    } 
  
  pthread_mutex_unlock(&positionReportMutex);
}
#endif
#if 0
int WalkPositionReport(int Tag, void *p, void *Parms)
{
  positionReport_t *Arg = p;
  char string[256];
  time_t tt = time(NULL);
  memset(&string, 0, sizeof(string));
  sprintf(string, "time : %s longitude : %x latitude : %x\r\n", ctime(&tt), Arg->longitude, Arg->latitude);


  printf("%s", string);

  return 0;
}
#endif

void buildPositionReportData(struct gps_fix_t *gpsData,   positionReport_t *report,   dataSendReq_t *dataSendReq)
{
    unsigned char DD;
    unsigned char MM;
    unsigned short SSSS;
    unsigned short dataLen = 0;
    
    memset(report, 0, sizeof(positionReport_t));
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

    printf("DD : %02x MM: %02x SSSS:%04x\r\n", DD, MM, SSSS);

    //report->speed = htons(gpsData->speed);
    report->speed = (gpsData->speed);
    //report->azimuth = htons(0);//TODO
    report->azimuth = (0);//TODO
    report->vehicleStatus = 0;//TODO
    report->directMark = getDriveDirect();
    report->nextStop = getNextStopId();
    report->nextStopDistance = htons(getDistance2NextStop(gpsData));
    report->cacheData = 0;
    report->mileage[0] = 0;//TODO
    report->mileage[1] = 0;//TODO
    report->mileage[2] = 0;//TODO
    report->overSpeed = 0;//TODO
    //report->temperature = htons(18);
    report->temperature = (18);
    //report->fuel1 = htons(30);
    report->fuel1 = (30);
    report->fuel2 = 80;
    report->operationStatus = 0;
    report->driverId = getDriverId();
    report->SIMType = 0;
    report->baseStatus = 0;
    report->basePosition = getBasePosition();
    report->baseCell = getBaseCell();

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
    memcpy(dataSendReq->data + dataLen, &report->nextStop, sizeof(report->nextStop));
    dataLen += sizeof(report->nextStop);
    memcpy(dataSendReq->data + dataLen, &report->nextStopDistance, sizeof(report->nextStopDistance));
    dataLen += sizeof(report->nextStopDistance);
    memcpy(dataSendReq->data + dataLen, &report->inStop, sizeof(report->inStop));
    dataLen += sizeof(report->inStop);
    memcpy(dataSendReq->data + dataLen, &report->cacheData, sizeof(report->cacheData));
    dataLen += sizeof(report->cacheData);
    memcpy(dataSendReq->data + dataLen, &report->mileage, sizeof(report->mileage));
    dataLen += sizeof(report->mileage);
    memcpy(dataSendReq->data + dataLen, &report->overSpeed, sizeof(report->overSpeed));
    dataLen += sizeof(report->overSpeed);
    memcpy(dataSendReq->data + dataLen, &report->temperature, sizeof(report->temperature));
    dataLen += sizeof(report->temperature);
    memcpy(dataSendReq->data + dataLen, &report->fuel1, sizeof(report->fuel1));
    dataLen += sizeof(report->fuel1);
    memcpy(dataSendReq->data + dataLen, &report->fuel2, sizeof(report->fuel2));
    dataLen += sizeof(report->fuel2);
    memcpy(dataSendReq->data + dataLen, &report->operationStatus, sizeof(report->operationStatus));
    dataLen += sizeof(report->operationStatus);
    strcpy(dataSendReq->data + dataLen, report->driverId);
    dataLen += strlen(report->driverId)+1;
    memcpy(dataSendReq->data + dataLen, &report->SIMType, sizeof(report->SIMType));
    dataLen += sizeof(report->SIMType);
    memcpy(dataSendReq->data + dataLen, &report->baseStatus, sizeof(report->baseStatus));
    dataLen += sizeof(report->baseStatus);
    strcpy(dataSendReq->data + dataLen, report->basePosition);
    dataLen += strlen(report->basePosition)+1;
    strcpy(dataSendReq->data + dataLen, report->baseCell);
    dataLen += strlen(report->baseCell)+1;

    dataSendReq->commandId = COMMAND_POSITION_REPORT;
    dataSendReq->dataLength = dataLen;
    printf("length of position report msg = %u\r\n", dataLen);

    return;

}


void FillReportAndSend(struct gps_fix_t* gpsData)
{
  /*
typedef struct
{
    U8  lng1;
    U8  lng2;
    U16  lng3;
    U8  lat1;
    U8  lat2;
    U16 lat3;
    U16 speed;//车速
    U16 azimuth;//方位角
    U8  vehicleStatus;//车辆状态
    U8  directMark;//上下行标志
    U8  nextStop;//下一站编号
    U16 nextStopDistance;//下一站距离
    U8  inStop;//是否在站内
    U8  cacheData;//是否缓存数据
    U8  mileage[3];//里程
    U8  overSpeed;//超速
    U16 temperature;//温度
    U16 fuel1;//油量整数
    U8  fuel2;//油量小数
    U8  operationStatus;//运营状态
    U8  *driverId;//司机ID
    U8  SIMType;//SIM卡类型
    U8  baseStatus;//基站定位：状态
    U8  *basePosition;//基站定位：位置
    U8  *baseCell;//基站定位：cellId
}positionReport_t;
   */
  positionReport_t report;
  dataSendReq_t dataSendReq;


  buildPositionReportData(gpsData, &report, &dataSendReq);
  dataSendReqSend(&dataSendReq);

#if 0

  memset(&report, 0, sizeof(positionReport_t));
  memset(&dataSendReq, 0, sizeof(dataSendReq_t));

  getDDMMSSSS(gpsData->longitude, &DD, &MM, &SSSS);
  report.longitude = (DD<<24) | (MM<<16) | (SSSS);
  getDDMMSSSS(gpsData->latitude, &DD, &MM, &SSSS);
  report.latitude = (DD<<24) | (MM<<16) | (SSSS);
  report.speed = gpsData->speed;
  report.azimuth = 0;//TODO
  report.vehicleStatus = 0;//TODO
  report.directMark = 0;//TODO
  report.nextStop = 0;//TODO
  report.nextStopDistance = 0;//TODO
  report.cacheData = 0;
  report.mileage = 0;//TODO
  report.overSpeed = 0;//TODO
  report.temperature = 18;
  report.fuel1 = 30;
  report.fuel2 = 80;
  report.operationStatus = 0;
  strcpy(report.driverId, "00001");
  report.SIMType = 0;
  report.baseStatus = 0;
  
  dataSendReq.commandId = 0x14;
  dataSendReq.dataLength = sizeof(positionReport_t);
  memcpy(&dataSendReq.data, &report, dataSendReq.dataLength);
  
  dataSendReqSend(&dataSendReq);

#endif
  
/*
  pthread_mutex_lock(&positionReportMutex);
  printf("333 num = %d\r\n", getNumOfList());
  DLAppend(&reportList, 0, &report, sizeof(positionReport_t));
  printf("444 num = %d\r\n", getNumOfList());
  pthread_mutex_unlock(&positionReportMutex);
*/
}

float getAngle(struct gps_fix_t *newestPoint, struct gps_fix_t *prevPoint)
{
  float dy = 0;
  float dx = 0;
  float angle = 0;
  if(NULL == newestPoint || NULL == prevPoint)
  {
    return 0;
  }
  dy = newestPoint->latitude - prevPoint->latitude;
  dx = cosf(M_PI/180*prevPoint->latitude)*(newestPoint->longitude - prevPoint->longitude);
  angle = atan2f(dy, dx);
//  printf("angle = %f\r\n", angle);
  return angle;

}

void *positionReportGetGPSDataUpdate(void *arg)
{
    pthread_mutex_lock(&GPSUpdateMutex);
	//printf("positionReport get sig of gps data update\r\n");
    GPSUpdateSignal = 1;
    pthread_mutex_unlock(&GPSUpdateMutex);
}

void* positionReport()
{
  struct gps_fix_t *newestPoint = NULL;
  struct gps_fix_t *prevPoint = NULL;
  static int inAngleCount = 0;
  unsigned long count = 0;
  int inAngle = 0;
  int period = 100000;//0.1 sec


  registerNoticeClientList(NOTICE_POSITION, NULL, positionReportGetGPSDataUpdate);

  for(;;)
  {
    newestPoint = NULL;
    prevPoint = NULL;
    usleep(period);
    count = count + 1;

    pthread_mutex_lock(&GPSUpdateMutex);
    if(GPSUpdateSignal == 1)
    {        
        //printf("count = %ld \r\n", count);
        newestPoint = GetNewestDataFirst(&gpsSource);
        prevPoint = GetNewestDataSecond(&gpsSource);
        GPSUpdateSignal = 0;
        if(count >= (100))//10 sec
        {
            FillReportAndSend(newestPoint);
            count = 0;
        }
    }
    pthread_mutex_unlock(&GPSUpdateMutex);

  }
}
