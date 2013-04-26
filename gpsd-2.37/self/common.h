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
	COMMAND_POSITION_REPORT = 0x14,
}COMMANDID;
#endif