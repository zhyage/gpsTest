#ifndef INTERNETCONNECTCHECK_H
#define INTERNETCONNECTCHECK_H

typedef enum
{
	DISCONNECTED = -1,
	CONNECTED	= 1,
}connectStatus_t;

typedef struct 
{
	int prevStatus;
	int currStatus;
}internetConnectStatus_t;

void* internetConnectCheck();

#endif