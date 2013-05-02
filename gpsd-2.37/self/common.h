#ifndef COMMON_H
#define COMMON_H

#define FALSE 0
#define TRUE  1

typedef int BOOL;
typedef char            S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
typedef long            S64;
typedef unsigned long   U64;


typedef enum 
{
	COMANDID_INTERNET_HANDSHAKE = 0x11,
	COMMAND_POSITION_REPORT = 0x14,
	COMMAND_ARRIVED_STOP_REPORT = 0x40,
	COMMAND_LEAVE_STOP_REPORT = 0x41,
	COMMAND_TEXT_INFO_PUSH	= 0x90,
	COMMAND_SCHEDULE_LINE_PUSH = 0x91,
	COMMAND_IN_OUT_PUSH = 0x98,
}COMMANDID;
#endif