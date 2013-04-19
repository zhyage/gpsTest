#ifndef GPSTEST_H
#define GPSTEST_H
#include "gps.h"
#include "position.h"
#include "stopAnnounce.h"

#define FALSE 0
#define TRUE  1
#define MISC_BUF_SIZE 10
typedef int BOOL;

typedef struct
{
    int  tail;
    int  head;
    struct gps_fix_t data[MISC_BUF_SIZE];
}gpsSourceData;

#endif
