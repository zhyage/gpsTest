#ifndef GPSTEST_H
#define GPSTEST_H
#include "gps.h"
//#include "position.h"
//#include "stopAnnounce.h"

#define FALSE 0
#define TRUE  1
#define MISC_BUF_SIZE 10


typedef int BOOL;
typedef char            S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
typedef long            S64;
typedef unsigned long   U64;

typedef struct
{
    int  tail;
    int  head;
    struct gps_fix_t data[MISC_BUF_SIZE];
}gpsSourceData;

#endif
