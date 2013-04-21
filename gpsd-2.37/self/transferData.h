#ifndef TRANSFERDATA_H
#define TRANSFERDATA_H
#include "dllist.h"

typedef enum
{
	POSITION_REPORT = 0,
	REPORT_END
}reportType;

typedef struct
{
	int reportType;
}reportSendNotic_t;

typedef struct
{
	DLLIST **list;
	void (*sendFunc)(DLLIST *list);
}reportListRegister_t;

void sendReportNotic(reportSendNotic_t notic );
void* transferData();
void registerReportList(int reportType, DLLIST *list, void *func);



#endif
