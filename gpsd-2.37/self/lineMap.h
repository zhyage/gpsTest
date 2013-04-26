#ifndef LINEMAP_H
#define LINEMAP_H

#define INVALID_ID -1
#define INVALID -1
#define VALID   1
#define UPLINE 1
#define DOWNLINE -1

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
    int     valid;
    double  lng;
    double  lat;
    int     lngAttr;
    int     latAttr;
    char    *arrivedMp3;
    char    *leavedMp3;
}spotMark_t;

typedef struct
{
    int id;
    char    *name;
    int type;//crooked, stop, hot
    spotMark_t  upline;
    spotMark_t  downline;    
}busStopMark_t;

/*
typedef struct
{
    int lineId;
    char *lineName;
    DLLIST *stopList;//stopId here
}lineData_t;

typedef struct
{
    int     cityId;
    char    *cityName;
    DLLIST *lineList;//lindId here
}allLineMark_t;
*/

typedef struct 
{
	int	lineId;
	char	*lineName;
	unsigned int	stopId[256];	
}lineData_t;

typedef struct
{
    int stopId;//current in which stop
    int upOrDown;//in up spot or down spot
    //stopPendAction_t *action;

}stopPend_t;

int getStopIdOfLine(int lineId, int num);
int getNextStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *nextStop);
int getPrevStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *prevStop);
busStopMark_t *getAllBusStop();
busStopMark_t *getBusStopBystopId(unsigned short stopId);
lineData_t *getLineData(unsigned int lineId);

#endif
