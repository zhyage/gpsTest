#ifndef STOP_ANNOUNCE_H
#define STOP_ANNOUNCE_H
#include "dllist.h"

#define INVALID_ID -1
#define INVALID -1
#define VALID   1

#define UPLINE 1
#define DOWNLINE -1

#define ARRIVE 1
#define LEAVE   -1

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

typedef struct
{
    char *mp3Name;
}stopPendAction_t;

typedef struct
{
    int stopId;//current in which stop
    int upOrDown;//in up spot or down spot
    //stopPendAction_t *action;

}stopPend_t;





void* stopAnnounce();
void addActionToActionPend(stopPendAction_t *action);
int getNextStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *nextStop);
int getPrevStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *prevStop);

#endif
