#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void getDDMMSSSS(float x, unsigned char *DD, unsigned char *MM, unsigned short *SSSS)
{
    if(0 == isnan(x))
    {
	   *DD = x;
	   *MM = (x * 100) - ((*DD)*100);
	   *SSSS = (x * 1000000) - ((*DD)*1000000) - ((*MM)*10000);
    }
    else
    {
        *DD = 0xFF;
        *MM = 0xFF;
        *SSSS = 0xFFFF;
    }
}

#define PI                      3.1415926
#define EARTH_RADIUS            6378.137        //地球近似半径


// 求弧度
double radian(double d)
{
    return d * PI / 180.0;   //角度1˚ = π / 180
}

//计算距离
double get_distance(double lat1, double lng1, double lat2, double lng2)
{
    double radLat1 = radian(lat1);
    double radLat2 = radian(lat2);
    double a = radLat1 - radLat2;
    double b = radian(lng1) - radian(lng2);
    
    double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));
    
    dst = dst * EARTH_RADIUS;
    //dst= round(dst * 10000) / 10000;
    return dst;
}

