#ifndef RECV_SESSION_H
#define RECV_SESSION_H
#include "common.h"

typedef struct
{
    U16 startTag;
    U16 length;
    U8  version;
    U8  sessionId;
    U8  motoId[16];
    U8  date[3];
    U8  time[3];
    U8  commandId;
    U8  data[128];
    U8  checkSum;    
}pushCommandData_t;

typedef struct 
{
    U8 lineName[64];
    U8 lineId[3]; 
}scheduleLineCommand_t;

typedef struct
{
    U8 inOrOut;
    U8 confirm;
}inOutCommand_t;

#endif