#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm/types.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include "recvSession.h"
#include "common.h"
#include "utils.h"
/*
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
*/

void pushCommand(unsigned char *data, unsigned short dataLen)
{
    static int sockfd=0;
        struct sockaddr_in servaddr;

        
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(9998);
        
        inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
        sockfd=socket(AF_INET,SOCK_DGRAM,0);

        sendto(sockfd, data,dataLen,
            0,(struct sockaddr *)&servaddr,sizeof(struct sockaddr));
}

void buildSendCommandPackageAndSend(unsigned int commandId, pushCommandData_t *commandPackage, 
    unsigned char *subData, unsigned short subDataLength)
{
    time_t currentTime = time(NULL);
    struct tm tt;
    unsigned short length = 0;
    unsigned char sendMsg[128];
    unsigned sendLen = 0;
    unsigned char *motoId = "bus 0001";

    localtime_r(&currentTime, &tt);

    printf("year: %d mon: %d day: %d hour: %d min: %d sec:%d\r\n"
        , tt.tm_year%100, 
        tt.tm_mon + 1, 
        tt.tm_mday,
        tt.tm_hour, 
        tt.tm_min, 
        tt.tm_sec);

    commandPackage->startTag = 0x2BD4;
    
    commandPackage->version = 0x12;
    length += sizeof(commandPackage->version);
    commandPackage->sessionId = 0;
    length += sizeof(commandPackage->sessionId);
    memcpy(commandPackage->motoId, motoId, strlen(motoId) + 1);
    length += strlen(commandPackage->motoId) + 1;
    commandPackage->date[0] = tt.tm_year % 100;
    commandPackage->date[1] = tt.tm_mon + 1;
    commandPackage->date[2] = tt.tm_mday;
    length += sizeof(commandPackage->date);
    commandPackage->time[0] = tt.tm_hour;
    commandPackage->time[1] = tt.tm_min;
    commandPackage->time[2] = tt.tm_sec;
    length += sizeof(commandPackage->time);
    commandPackage->commandId = commandId;
    length += sizeof(commandPackage->commandId);
    memcpy(commandPackage->data, subData, subDataLength);
    length += subDataLength;
    commandPackage->length = length;
    commandPackage->checkSum = 0;//don't know yet

    memcpy(sendMsg + sendLen, &commandPackage->startTag, sizeof(commandPackage->startTag));
    sendLen += sizeof(commandPackage->startTag);

    memcpy(sendMsg + sendLen, &commandPackage->length, sizeof(commandPackage->length));
    sendLen += sizeof(commandPackage->length);

    memcpy(sendMsg + sendLen, &commandPackage->version, sizeof(commandPackage->version));
    sendLen += sizeof(commandPackage->version);

    memcpy(sendMsg + sendLen, &commandPackage->sessionId, sizeof(commandPackage->sessionId));
    sendLen += sizeof(commandPackage->sessionId);

    memcpy(sendMsg + sendLen, commandPackage->motoId, strlen(commandPackage->motoId) + 1);
    sendLen += strlen(commandPackage->motoId) + 1;

    memcpy(sendMsg + sendLen, commandPackage->date, sizeof(commandPackage->date));
    sendLen += sizeof(commandPackage->date);

    memcpy(sendMsg + sendLen, commandPackage->time, sizeof(commandPackage->time));
    sendLen += sizeof(commandPackage->time);

    memcpy(sendMsg + sendLen, &commandPackage->commandId, sizeof(commandPackage->commandId));
    sendLen += sizeof(commandPackage->commandId);

    memcpy(sendMsg + sendLen, commandPackage->data, subDataLength);
    sendLen += subDataLength;

    commandPackage->checkSum = getCheckSum((sendMsg + 
        sizeof(commandPackage->startTag) + 
        sizeof(commandPackage->length)), commandPackage->length);

    memcpy(sendMsg + sendLen, &commandPackage->checkSum, sizeof(commandPackage->checkSum));
    sendLen += sizeof(commandPackage->checkSum);

    if(1)
    {
        FILE *pushCmdFd;

        pushCmdFd = fopen("pushCmd.log", "a");

        fwrite(sendMsg, 1, sendLen, pushCmdFd);

        fclose(pushCmdFd);
    }

    pushCommand(sendMsg, sendLen);
}


unsigned short buildscheduleLineCommand(unsigned int lineId, unsigned char *lineName, 
    scheduleLineCommand_t *scheduleCommandPackage)
{
    unsigned short length = 0;
    strcpy(scheduleCommandPackage->lineName, lineName);
    length += strlen(lineName) + 1;
    scheduleCommandPackage->lineId[0] = lineId >> 16;
    scheduleCommandPackage->lineId[1] = lineId >> 8;
    scheduleCommandPackage->lineId[2] = lineId;
    length += 3;

    return length;

}

unsigned short buildInOutCommandPackage(unsigned char inOrOut, unsigned char confirm, 
    inOutCommand_t *inOutCommandPackage)
{
    unsigned short length = 0;
    inOutCommandPackage->inOrOut = inOrOut;
    inOutCommandPackage->confirm = confirm;
    length += 2;
    return length;
}

void buildSendCommandScheduleLinePackage(unsigned int lineId, unsigned char *lineName)
{
    scheduleLineCommand_t scheduleCmd;
    pushCommandData_t pushCmd;
    unsigned short subCmdLength = 0;
    
    subCmdLength = buildscheduleLineCommand(lineId, lineName, &scheduleCmd);
    buildSendCommandPackageAndSend(COMMAND_SCHEDULE_LINE_PUSH, &pushCmd, (unsigned char*)&scheduleCmd, subCmdLength);

}

void buildSendCommandInOutPackage(unsigned char inOrOut, unsigned char confirm)
{
    inOutCommand_t inOutCmd;
    pushCommandData_t pushCmd;
    unsigned short subCmdLength = 0;
    
    subCmdLength = buildInOutCommandPackage(inOrOut, confirm, &inOutCmd);
    buildSendCommandPackageAndSend(COMMAND_IN_OUT_PUSH, &pushCmd, (unsigned char*)&inOutCmd, subCmdLength);

}


void *serverCommandInput()
{
    char command = 0;
    static int line = 0;
    static int inOut = 0;
    printf("input '1' for send COMMAND_TEXT_INFO_PUSH\r\n");
    printf("input '2' for send COMMAND_SCHEDULE_LINE_PUSH\r\n");
    printf("input '3' for send COMMAND_IN_OUT_PUSH\r\n");


    while(1)
    {
        command = getchar();
        switch (command)
        {
            case 49:
            {
                printf("send COMMAND_TEXT_INFO_PUSH\r\n");
            }
            break;
            case 50:
            {
                printf("send COMMAND_SCHEDULE_LINE_PUSH\r\n");
                if(line == 0)
                {
                    buildSendCommandScheduleLinePackage(1, "zhi_jiang_dong_lu");
                    line = 1;
                }
                else
                {
                    buildSendCommandScheduleLinePackage(2, "yan_jiang_da_dao");
                    line = 0;
                }
            }
            break;
            case 51:
            {
                printf("send COMMAND_IN_OUT_PUSH\r\n");
                if(inOut == 0)
                {
                    buildSendCommandInOutPackage(0x00, 0x00);
                    inOut = 1;
                }
                else
                {
                    buildSendCommandInOutPackage(0x01, 0x00);
                    inOut = 0;
                }
            }
            break;
            default:
            {
                //printf("unknow command\r\n");
            }
            break;
        }
    }
}

int main()
{
	int period = 100000;//0.1 sec
    struct timeval timeout;
    static struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	fd_set set;
    int s = 0;
    int sock_opt = 1;
    pthread_t serverCommandInput_id;


    pthread_create(&serverCommandInput_id, NULL, serverCommandInput, NULL);
    
  
    s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0){
		printf("error to get socket\n");
		return -1;
	}
	sock_opt = 1;

	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(void *)&sock_opt, sizeof(sock_opt)) == -1)
	{
			printf("error to set sock opt reuseaddr\n");
			return -1;

	}

			
	memset((char *)&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	//inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);
	serv_addr.sin_port = htons(9999);
	

	if (bind (s, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
	{
		printf("error to binding 9999\n");
		close(s);
		return -1;
	}


    for(;;)
	{
		FD_ZERO(&set);
		FD_SET(s,&set);
		timeout.tv_sec=0;
		timeout.tv_usec=period;
        unsigned char recvBuf[128];
        unsigned char *pos;
        int n = 0;
   
       	memset(recvBuf, 0, 128);
        select(FD_SETSIZE,&set,NULL,NULL,&timeout);
        
        
        if (FD_ISSET(s, &set)) 
        {
        	U16 *startTag = NULL;
        	U16 *length = NULL;
        	U8 *version = NULL;
        	U8 *sessionId = NULL;
        	U8 *checkLineStatus = NULL;
        	U16 *reserve = NULL;
        	U8	*motoType = NULL;
        	U8	*motoId = NULL;
        	U8	*date0 = NULL;
        	U8	*date1 = NULL;
        	U8	*date2 = NULL;
        	U8	*time0 = NULL;
        	U8	*time1 = NULL;
        	U8	*time2 = NULL;
        	U8	*commandId = NULL;

			n=recvfrom(s, recvBuf, 128, 0, (struct sockaddr *)&cli_addr, &clilen);
            printf("server recv data length =  %d\r\n", n);
            

            pos = recvBuf;

            startTag = (U16 *)pos;
            pos = pos + 2;
            length = (U16 *)(pos);
            pos = pos + 2;
            version = (U8 *)(pos);
            pos = pos + 1;
            sessionId = (U8 *)(pos);
            pos = pos + 1;
            checkLineStatus = (U8 *)(pos);
            pos = pos + 1;
            reserve = (U16 *)(pos);
            pos = pos + 2;
            motoType = (U8 *)(pos);
            pos = pos + 1;
            motoId = (U8 *)(pos);
            pos = pos + strlen(motoId) + 1;
            date0 = (U8 *)(pos);
            pos = pos + 1;
            date1 = (U8 *)(pos);
            pos = pos + 1;
            date2 = (U8 *)(pos);
            pos = pos + 1;
            time0 = (U8 *)(pos);
            pos = pos + 1;
            time1 = (U8 *)(pos);
            pos = pos + 1;
            time2 = (U8 *)(pos);
            pos = pos + 1;
            commandId = (U8 *)(pos);

            printf("---------------------------------------------------\r\n");
            printf("startTag = %04x\r\n", (*startTag));
            printf("length = %d\r\n", (*length));
            printf("version = %02x\r\n", (*version));
            printf("sessionId = %d\r\n", (*sessionId));
            printf("checkLineStatus = %02x\r\n", (*checkLineStatus));
            printf("reserve = %04x\r\n", (*reserve));
            printf("motoType = %d\r\n", (*motoType));
            printf("motoId = %s\r\n", motoId);
            printf("date0 date1 date2 = %d:%d:%d\r\n", (*date0), (*date1), (*date2) );
            printf("time0 time1 time2 = %d:%d:%d\r\n", (*time0), (*time1), (*time2) );
            printf("commandId = %02x\r\n", (*commandId));
            



        }
        


    }
}
