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
#include "dllist.h"
#include "sendSession.h"
#include "identity.h"

SendList_t g_noResSendList = {0, NULL};//save sendData_t

SendList_t g_needResSendList = {0, NULL};//save sendData_t

commandAttr_t g_commandDefine[256] = {0};


void addCommandAttr2Define(commandAttr_t *cmdAttr)
{
    memcpy(&g_commandDefine[cmdAttr->commandId], cmdAttr, sizeof(commandAttr_t));
}

commandAttr_t *getCommandAttrDefine(unsigned char commandId)
{
    commandAttr_t *cmdAttr = &g_commandDefine[commandId];
    if(cmdAttr->commandId != commandId)
    {
        return NULL;
    }
    return cmdAttr;
}

void initCommandDefine()
{
    commandAttr_t cmdAttr;
    memset(g_commandDefine, 0, sizeof(commandAttr_t) * 256);
    
    cmdAttr.commandId = COMMAND_POSITION_REPORT;
    strcpy(cmdAttr.commandName, "position cycle report");
    cmdAttr.sendOrRecv = SEND_COMMAND;
    cmdAttr.needResponse = NO_NEED_RES;
    addCommandAttr2Define(&cmdAttr);

    cmdAttr.commandId = COMMAND_ARRIVED_STOP_REPORT;
    strcpy(cmdAttr.commandName, "arrived stop report");
    cmdAttr.sendOrRecv = SEND_COMMAND;
    cmdAttr.needResponse = NO_NEED_RES;
    addCommandAttr2Define(&cmdAttr);

    cmdAttr.commandId = COMMAND_LEAVE_STOP_REPORT;
    strcpy(cmdAttr.commandName, "leave stop report");
    cmdAttr.sendOrRecv = SEND_COMMAND;
    cmdAttr.needResponse = NO_NEED_RES;
    addCommandAttr2Define(&cmdAttr);

    cmdAttr.commandId = COMANDID_INTERNET_HANDSHAKE;
    strcpy(cmdAttr.commandName, "internet connect hand report");
    cmdAttr.sendOrRecv = SEND_COMMAND;
    cmdAttr.needResponse = NO_NEED_RES;
    addCommandAttr2Define(&cmdAttr);
        
}

void printAllCommandDefine()
{
    int i = 0;
    for(i = 0; i < 256; i++)
    {
        commandAttr_t *cmdAttr = &g_commandDefine[i];
        if(0 != cmdAttr->commandId)
        {
            printf("command Id : %d command name : %s sendOrRecv : %d needResponse : %d\r\n", 
                cmdAttr->commandId, cmdAttr->commandName, cmdAttr->sendOrRecv, cmdAttr->needResponse);
        }
    }
}

unsigned int getSessionId()
{
    static unsigned int sessionId = 0;
    sessionId = sessionId + 1;
    if(sessionId >= 65534)
    {
        sessionId = 0;
    }
    return sessionId;
}

void addData2SendList(SendList_t *sList, sendData_t* sendData)
{
    if(sList->eleNum < MAX_SEND_BUF)
    {
        DLAppend(&(sList->sendList), 0, sendData, sizeof(sendData_t));
        sList->eleNum = sList->eleNum + 1;
    }
    else//delete the oldest one, the add new one
    {
        DLLIST *oldestItem = DLGetFirst(sList->sendList);
        printf("sendList full---------------------\r\n");
        if(NULL == oldestItem)
        {
            return;
        }
        DLLIST *nextItem = oldestItem->Next;
        if(NULL == nextItem)
        {
            return;
        }
        sList->sendList = nextItem;
        DLDelete(oldestItem);
        DLAppend(&(sList->sendList), 0, sendData, sizeof(sendData_t));
    }
    return;
}

void addData2needResSendList(sendData_t* sendData)
{
    addData2SendList(&g_needResSendList, sendData);
}

void addData2noResSendList(sendData_t* sendData)
{
    addData2SendList(&g_noResSendList, sendData);
}

void AddSendData2List(dataSendReq_t *dataSendReq)
{
    sendData_t sendData;
    commandAttr_t *cmdAttr = getCommandAttrDefine(dataSendReq->commandId);
    if(NULL == cmdAttr || cmdAttr->sendOrRecv != SEND_COMMAND)
    {
        return;
    }
    
    sendData.commandAttr = cmdAttr;
    sendData.sessionId = getSessionId();
    sendData.time = time(NULL);
    sendData.retryTimes = 0;
    sendData.dataLength = dataSendReq->dataLength;
    memcpy(sendData.data, dataSendReq->data, dataSendReq->dataLength);
    
    if(cmdAttr->needResponse == NEED_RES)
    {
        addData2needResSendList(&sendData);
    }
    else
    {
        addData2noResSendList(&sendData);
    }
    printf("eleNum of g_needResSendList = %d\r\n", g_needResSendList.eleNum);
    printf("eleNum of g_noResSendList = %d\r\n", g_noResSendList.eleNum);
    
}

int sendDataEle(sendData_t *sendData)
{
    printf("sendDataEle *********************************\r\n");
    printf("command Id : %d\r\n", sendData->commandAttr->commandId);
    printf("command name : %s\r\n", sendData->commandAttr->commandName);
    printf("command needRes : %d\r\n", sendData->commandAttr->needResponse);
    printf("sessionId : %d\r\n", sendData->sessionId);
    printf("retry times : %d\r\n", sendData->retryTimes);
    printf("data length : %u\r\n", sendData->dataLength);
 
    return buildAndSendUploadData(sendData);
}

void sendSendList(SendList_t *sList, int needRes)
{
    DLLIST *item;
    for(item = DLGetFirst(sList->sendList); item != NULL; item = item->Next)
    {
        sendData_t *sendData = item->Object;
        if(1 == sendDataEle(sendData))
        {
            item->Tag = SENDOK;//send success
        }
        else
        {
            item->Tag = SENDERR;//send fail
        }
        sendData->retryTimes = sendData->retryTimes + 1;
    }
    
    if(NO_NEED_RES == needRes)//if no need res, delete when it send success
    {
        for(item = DLGetFirst(sList->sendList); item != NULL;)
        {
            DLLIST *nextItem = item->Next;
            if(item != NULL && item->Tag == SENDOK)
            {
                if(item == DLGetFirst(sList->sendList))
                {
                    sList->sendList = item->Next;
                    DLDelete(item);
                    sList->eleNum = sList->eleNum - 1;
                    item = sList->sendList;
                    continue;
                }
                else
                {
                    DLDelete(item);
                    sList->eleNum = sList->eleNum - 1;
                }
            }
            item = nextItem;
        }
    }
}

void sendData2Remote()
{
    sendSendList(&g_needResSendList, NEED_RES);
    sendSendList(&g_noResSendList, NO_NEED_RES);
}


void* sendSession()
{

    int period = 100000;//0.1 sec
    struct timeval timeout;
    static struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	fd_set set;
    int s = 0;
    int sock_opt = 1;
    
    initCommandDefine();
    printAllCommandDefine();
  
    s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0){
		printf("error to get socket\n");
		return ;
	}
	sock_opt = 1;

	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(void *)&sock_opt, sizeof(sock_opt)) == -1)
	{
			printf("error to set sock opt reuseaddr\n");
			return ;

	}

			
	memset((char *)&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);
	serv_addr.sin_port = htons(9920);
	

	if (bind (s, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
	{
		printf("error to binding 9920\n");
		close(s);
		return ;
	}


    for(;;)
	{
		FD_ZERO(&set);
		FD_SET(s,&set);
		timeout.tv_sec=0;
		timeout.tv_usec=period;
        dataSendReq_t req;
        int n = 0;
   
       
        select(FD_SETSIZE,&set,NULL,NULL,&timeout);
        
        
        if (FD_ISSET(s, &set)) 
        {
			n=recvfrom(s, &req, sizeof(dataSendReq_t), 0, (struct sockaddr *)&cli_addr, &clilen);
            printf("recv dataSendReq commandId %d\r\n", req.commandId);
            AddSendData2List(&req);
        }

        sendData2Remote();

    }
}


void dataSendReqSend(dataSendReq_t *dataSendReq )
{
		static int sockfd=0;
		struct sockaddr_in servaddr;

		
		bzero(&servaddr,sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(9920);
		
		inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
        if(sockfd == 0)
        {
		  sockfd=socket(AF_INET,SOCK_DGRAM,0);
        }
		sendto(sockfd, dataSendReq,sizeof(dataSendReq_t),
			0,(struct sockaddr *)&servaddr,sizeof(struct sockaddr));
}

int send2Remote(unsigned char *data, unsigned short length )
{
        static int sockfd=0;
        struct sockaddr_in servaddr;
        int n = 0;

        
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(9999);
        
        //inet_pton(AF_INET,"111.13.47.157",&servaddr.sin_addr);
        inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
        if(0 == sockfd)
        {
            sockfd=socket(AF_INET,SOCK_DGRAM,0);
        }
        n = sendto(sockfd, data, length,
            0,(struct sockaddr *)&servaddr,sizeof(struct sockaddr));
        printf("sendto length = %d\r\n", n);
        
        if(n == -1 || n != length)
        {
            printf("err to send2Remote\r\n");
            printf("err reason : %s\r\n", strerror(errno));
            return -1;
        }
        
        return 1;
}

/*
typedef struct
{
    U16 startTag;
    U16 length;
    U8  version;
    U8  sessionId;
    U8  checkLineStatus;
    U16 reserve;
    U8  motoType;
    U8  *motoId;
    U8  date[3];
    U8  time[3];
    U8  commandId;
    U8  *data;
    U8  checkSum;    
}uploadData_t;
*/




int buildAndSendUploadData(sendData_t *sendData)
{
    unsigned short packageLength = 0;
    unsigned short msgLen = 0;
    unsigned char sendMsg[128];
    uploadData_t updateData;
    struct tm time;
//    FILE *positionLogFd;

//    positionLogFd = fopen("positionLog.log", "a");
    
    localtime_r(&(sendData->time), &time);
    
    printf("year: %d mon: %d day: %d hour: %d min: %d sec:%d\r\n"
        , time.tm_year%100, 
        time.tm_mon + 1, 
        time.tm_mday,
        time.tm_hour, 
        time.tm_min, 
        time.tm_sec);

    packageLength = sizeof(updateData.version) + 
                    sizeof(updateData.sessionId) +
                    sizeof(updateData.checkLineStatus) +
                    sizeof(updateData.reserve) +
                    sizeof(updateData.motoType) +
                    (strlen(getMotoId()) + 1) + //motoId
                    sizeof(updateData.date) +
                    sizeof(updateData.time) +
                    sizeof(updateData.commandId) +
                    sendData->dataLength;
                    
    printf("upload package length = %u\r\n", packageLength);                

    //updateData.startTag = htons(0x1AE6);
    updateData.startTag = (0x1AE6);
    //updateData.length = htons(packageLength);
    updateData.length = (packageLength);
    updateData.version = 0x12;
    updateData.sessionId = sendData->sessionId;
    updateData.checkLineStatus = 0;
    //updateData.reserve = htons(0x0000);
    updateData.reserve = (0x0000);
    updateData.motoType = 0x71;
    updateData.motoId = getMotoId();
    updateData.date[0] = time.tm_year % 100;
    updateData.date[1] = time.tm_mon + 1;
    updateData.date[2] = time.tm_mday;
    updateData.time[0] = time.tm_hour;
    updateData.time[1] = time.tm_min;
    updateData.time[2] = time.tm_sec;
    updateData.commandId = sendData->commandAttr->commandId;
    updateData.data = sendData->data;
    updateData.checkSum = 0;
       
    memcpy(sendMsg + msgLen, &updateData.startTag, sizeof(updateData.startTag));
    msgLen += sizeof(updateData.startTag);
    memcpy(sendMsg + msgLen, &updateData.length, sizeof(updateData.length));
    msgLen += sizeof(updateData.length);

    memcpy(sendMsg + msgLen, &updateData.version, sizeof(updateData.version));
    msgLen += sizeof(updateData.version);
    memcpy(sendMsg + msgLen, &updateData.sessionId, sizeof(updateData.sessionId));
    msgLen += sizeof(updateData.sessionId);
    memcpy(sendMsg + msgLen, &updateData.checkLineStatus, sizeof(updateData.checkLineStatus));
    msgLen += sizeof(updateData.checkLineStatus);
    memcpy(sendMsg + msgLen, &updateData.reserve, sizeof(updateData.reserve));
    msgLen += sizeof(updateData.reserve);
    memcpy(sendMsg + msgLen, &updateData.motoType, sizeof(updateData.motoType));
    msgLen += sizeof(updateData.motoType);
    memcpy(sendMsg + msgLen, updateData.motoId, strlen(updateData.motoId) + 1);
    msgLen += strlen(updateData.motoId) + 1;

    memcpy(sendMsg + msgLen, &updateData.date, sizeof(updateData.date));
    msgLen += sizeof(updateData.date);
    memcpy(sendMsg + msgLen, &updateData.time, sizeof(updateData.time));
    msgLen += sizeof(updateData.time);
    memcpy(sendMsg + msgLen, &updateData.commandId, sizeof(updateData.commandId));
    msgLen += sizeof(updateData.commandId);

    memcpy(sendMsg + msgLen, updateData.data, sendData->dataLength);
    msgLen += sendData->dataLength;

    updateData.checkSum = getCheckSum((sendMsg + 
        sizeof(updateData.startTag) + 
        sizeof(updateData.length)), packageLength);

    memcpy(sendMsg + msgLen, &updateData.checkSum, sizeof(updateData.checkSum));
    msgLen += sizeof(updateData.checkSum);

//    fwrite(sendMsg, 1, msgLen, positionLogFd);

//    fclose(positionLogFd);

    return send2Remote(sendMsg, msgLen);

}
