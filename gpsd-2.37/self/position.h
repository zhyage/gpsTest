#ifndef POSITION_H
#define POSITION_H


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
}positionReport_t;

void* positionReport();
int WalkPositionReport(int Tag, void *p, void *Parms);
void FillReportAddToList(struct gps_fix_t* gpsData);



#endif
