#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "gpsTest.h"
#include "arrQueue.h"
#include "position.h"
#include "dllist.h"
#include "transferData.h"

extern gpsSourceData gpsSource;

DLLIST *reportList = NULL;

int WalkPositionReport(int Tag, void *p, void *Parms)
{
    struct positionReport_t *Arg = p;
    
    printf("in report = %f\r\n", Arg->latitude);
    
  return 0;
}


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

	memset(&report, 0, sizeof(positionReport_t));
	
	report.commandId = 0x14;
	report.longitude = gpsData->longitude;
	report.latitude = gpsData->latitude;
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

	DLAppend(&reportList, 0, &report, sizeof(struct positionReport_t));
	
	
	DLWalk(reportList, WalkReport, NULL);
    
}



void* positionReport()
{
    struct gps_fix_t *newest = NULL;
    struct gps_fix_t *second = NULL;
	reportSendNotic_t notic;

	notic.reportType = POSITION_REPORT;
    
    for(;;)
    {
        sleep(5);
        
        printf("positionReport \r\n");
        
        newest = GetNewestDataFirst(&gpsSource);
        second = GetNewestDataSecond(&gpsSource);
        
		FillReportAddToList(newest);

		if(1)
		{
			sendReportNotic(notic);
		}

        
		
    }
}
