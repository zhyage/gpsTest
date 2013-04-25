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

SendList_t g_noResSendList = {0, NULL};//save sendData_t

SendList_t g_needResSendList = {0, NULL};//save sendData_t

commandAttr_t g_commandDefine[256] = {0};

unsigned char *g_motoId = "bus 0001";

unsigned char *getMotoId()
{
    return g_motoId;
}

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
    
    cmdAttr.commandId = 0x14;
    strcpy(cmdAttr.commandName, "position cycle report");
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
    sendData.time = ctime(NULL);
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
    
    return 1;
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
	inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);
	serv_addr.sin_port = htons(9920);
	

	if (bind (s, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
	{
		printf("error to binding 9920\n");
		close(s);
		return -1;
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
            AddSendData2List(&req);
            AddSendData2List(&req);
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
		sockfd=socket(AF_INET,SOCK_DGRAM,0);

		sendto(sockfd, dataSendReq,sizeof(dataSendReq_t),
			0,(struct sockaddr *)&servaddr,sizeof(struct sockaddr));
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

void buildUploadData(sendData_t *sendData)
{
    unsigned short packageLength = 0;
    uploadData_t updateData;
    struct tm time;
    
    localtime_r(&(sendData->time), &time);

    packageLength = sizeof(updateData.version) + 
                    sizeof(updateData.sessionId) +
                    sizeof(updateData.checkLineStatus) +
                    sizeof(updateData.reserve) +
                    sizeof(updateData.motoType) +
                    (strlen(getMotoId()) + 1) + //motoId
                    sizeof(updateData.date) +
                    sizeof(updateData.time) +
                    sizeof(updateData.commandId) +
                    sendData->dataLength + 
                    sizeof(updateData.checkSum) ;
                    
    printf("upload package length = %u\r\n", packageLength);                

    updateData.startTag = 0x1AE6;
    updateData.length = packageLength;
    updateData.version = 0x12;
    updateData.sessionId = sendData->sessionId;
    updateData.checkLineStatus = 0;
    updateData.reserve = 0x00;
    updateData.motoType = 0x71;
    updateData.data[0] = time.tm_year % 2000;
    updateData.data[1] = time.tm_mon;
    updateData.data[2] = time.tm_mday;
    updateData.time[0] = time.tm_hour;
    updateData.time[1] = time.tm_min;
    updateData.time[2] = time.tm_sec;
    updateData.commandId = sendData->commandAttr->commandId;
    
    
    
}
