#ifndef GPSTEST_H
#define GPSTEST_H
#include "gps.h"
//#include "position.h"
//#include "stopAnnounce.h"


#define MISC_BUF_SIZE 10




typedef struct
{
    int  tail;
    int  head;
    struct gps_fix_t data[MISC_BUF_SIZE];
}gpsSourceData;

typedef enum
{
	NOTICE_POSITION = 0,
	NOTICE_ANNOUNCE,		
	NOTICE_END
}noticClient_t;

typedef struct
{
	void *arg;
	void (*sendFunc)(void *arg);
}locationUpdateRegister_t;

void registerNoticeClientList(int noticeClent, void *arg, void *func);



#endif
