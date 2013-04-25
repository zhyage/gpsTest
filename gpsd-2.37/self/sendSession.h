#ifndef SEND_SESSION_H
#define SEND_SESSION_H

#define SEND_COMMAND 1
#define RECV_COMMAND -1

#define NEED_RES 1
#define NO_NEED_RES -1

#define MAX_SEND_BUF  128

typedef struct
{
    unsigned char commandId;
    char commandName[64];
    char sendOrRecv;
    char needResponse;
}commandAttr_t;

typedef struct
{
    commandAttr_t *commandAttr;
    unsigned short sessionId;
    time_t time;
    unsigned char retryTimes;
    unsigned int dataLength;
    char data[1024];
}sendData_t;

typedef struct
{
    unsigned char commandId;
    unsigned int dataLength;
    void *data;
}dataSendReq_t;

typedef struct
{
    unsigned short eleNum;
    DLLIST *sendList;
    
}SendList_t;




#endif