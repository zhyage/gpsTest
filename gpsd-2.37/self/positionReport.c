#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include "gpsTest.h"
#include "arrQueue.h"
#include "position.h"
#include "dllist.h"
#include "transferData.h"
#include "utils.h"

pthread_mutex_t	positionReportMutex = PTHREAD_MUTEX_INITIALIZER;

extern gpsSourceData gpsSource;
FILE *posFd = NULL;

DLLIST *reportList = NULL;

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

void *sendPositionReport(DLLIST *list)
{
	pthread_mutex_lock(&positionReportMutex);
	printf("111 num = %d\r\n", getNumOfList());
	DLLIST *thisItem;
	for(thisItem = list; thisItem != NULL; thisItem = thisItem->Next)
	{
    		char string[256];
		time_t tt = time(NULL);
		positionReport_t *arg = thisItem->Object;
		memset(&string, 0, sizeof(string));
		sprintf(string, "time : %s longitude : %x latitude : %x\r\n", ctime(&tt), arg->longitude, arg->latitude);
		thisItem->Tag = 1;//already sent
		fputs(string, posFd);
		fflush(posFd);
	}
	for(thisItem = list; thisItem != NULL; thisItem = thisItem->Next)
	{
		if(thisItem->Tag == 1)
		{
			if(NULL == thisItem->Next && NULL == thisItem->Prev)
			{
				reportList = NULL;
			}
			DLDelete(thisItem);
		}
	}
	printf("222 num = %d\r\n", getNumOfList());
	pthread_mutex_unlock(&positionReportMutex);
}

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

void FillReportAddToList(struct gps_fix_t* gpsData)
{
/*
typedef struct
{
	U8  commandId;//命令字
    U32 longitude;//经度
    U32 latitude;//纬度
    U16 speed;//车速
    U16 azimuth;//方位角
    U8  vehicleStatus;//车辆状态
    U8  directMark;//上下行标志
    U8  nextStop;//下一站编号
    U16 nextStopDistance;//下一站距离
    U8  inStop;//是否在站内
    U8  cacheData;//是否缓存数据
    U32 mileage;//里程(TODO U24???)
    U8  overSpeed;//超速
    U16 temperature;//温度
    U16 fuel1;//油量整数
    U8  fuel2;//油量小数
    U8  operationStatus;//运营状态
    U8  driverId[16];//司机ID
    U8  SIMType;//SIM卡类型
    U8  baseStatus;//基站定位：状态
    U8  basePosition[16];//基站定位：位置
    U8  baseCell[16];//基站定位：cellId
}pos

*/
    positionReport_t report;
    unsigned int DD;
    unsigned int MM;
    unsigned int SSSS;



	memset(&report, 0, sizeof(positionReport_t));
	
	report.commandId = 0x14;
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

	pthread_mutex_lock(&positionReportMutex);
	printf("333 num = %d\r\n", getNumOfList());
	DLAppend(&reportList, 0, &report, sizeof(positionReport_t));
	printf("444 num = %d\r\n", getNumOfList());
	pthread_mutex_unlock(&positionReportMutex);
    
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
	return angle;
	
}

void* positionReport()
{
    struct gps_fix_t *newestPoint = NULL;
    struct gps_fix_t *prevPoint = NULL;
	static int inAngleCount = 0;
	static int count = 0;
	int inAngle = 0;
	reportSendNotic_t notic;
	
	posFd = fopen("positionReport.log", "a+");

	notic.reportType = POSITION_REPORT;
	//registerReportList(POSITION_REPORT, &reportList);
	registerReportList(POSITION_REPORT, &reportList, sendPositionReport);
    
    for(;;)
    {
        sleep(1);
	count = count + 1;
	
        
        printf("positionReport \r\n");
        
        newestPoint = GetNewestDataFirst(&gpsSource);
        prevPoint = GetNewestDataSecond(&gpsSource);

	if(10 < getAngle(newestPoint, prevPoint))
	{
		inAngleCount = inAngleCount + 1;
	}
	else
	{
		inAngleCount = 0;
	}

	if(count >= 10 || inAngleCount >= 3)
	{
        
		FillReportAddToList(newestPoint);
		sendReportNotic(notic);
		count = 0;
	}

        
		
    }
}
