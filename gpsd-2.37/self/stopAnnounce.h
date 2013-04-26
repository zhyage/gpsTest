#ifndef STOP_ANNOUNCE_H
#define STOP_ANNOUNCE_H
#include "dllist.h"
#include "lineMap.h"

#define INVALID_ID -1
#define INVALID -1
#define VALID   1

#define UPLINE 1
#define DOWNLINE -1

#define ARRIVE 1
#define LEAVE   -1



typedef struct
{
    char *mp3Name;
}stopPendAction_t;







void* stopAnnounce();
void addActionToActionPend(stopPendAction_t *action);
int getNextStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *nextStop);
int getPrevStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *prevStop);
int getDriveDirect();
int getLastStopId();
busStopMark_t *getNextStopAttr();
spotMark_t *getNextStopSpotAttr();
unsigned short getDistance2NextStop(struct gps_fix_t *current);
int getNextStopId();

#endif
