#ifndef TRANSFERDATA_H
#define TRANSFERDATA_H

typedef enum
{
	POSITION_REPORT = 0,
	REPORT_END
}reportType;

typedef struct
{
	int reportType;
}reportSendNotic_t;

void sendReportNotic(reportSendNotic_t notic );
void* transferData();



#endif
