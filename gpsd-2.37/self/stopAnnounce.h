#ifndef STOP_ANNOUNCE_H
#define STOP_ANNOUNCE_H
#include "dllist.h"
#include "lineMap.h"
#include "sendSession.h"

#define ARRIVE 1
#define LEAVE   -1




typedef struct
{
    char *mp3Name;
}stopPendAction_t;


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
    U8  stopId;
    U8  lineId[3];
    U8  *lineName;
    U8  *driverId;
}arrivedLeaveStopReport_t;

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
    U8  *softVersion;
    U8  *deviceId;
    U8  *lineName;
    U8  lineId[3];
    U8  *license;
    U8  *SIMId;
}internetHandShakeReport_t;






void* stopAnnounce();
void addActionToActionPend(stopPendAction_t *action);
int getNextStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *nextStop);
int getPrevStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *prevStop);
int getDriveDirect();
int getLastStopId();
busStopMark_t *getNextStopAttr();
spotMark_t *getNextStopSpotAttr();
unsigned short getDistance2NextStop(struct gps_fix_t *current);
void buildArrivedLeaveReportData(struct gps_fix_t *gpsData,   arrivedLeaveStopReport_t *report,   dataSendReq_t *dataSendReq, int arrivedOrLeave);
int getNextStopId();
void FillArrivedLeaveReportAndSend(struct gps_fix_t* gpsData, int arriveOrLeave);
void buildInternetHandShakeReportData(struct gps_fix_t *gpsData,   internetHandShakeReport_t *report,   dataSendReq_t *dataSendReq);
void FillInternetHandShakeReportAndSend(struct gps_fix_t* gpsData);
busStopMark_t *getCurrentStopAttr();
spotMark_t *getCurrentSpotAttr();
unsigned short getDistance2CurrentStop(struct gps_fix_t *current);
#endif
