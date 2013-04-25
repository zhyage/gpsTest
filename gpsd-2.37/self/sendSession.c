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
        int recvCommand = 0;
        int n = 0;
   
       
        select(FD_SETSIZE,&set,NULL,NULL,&timeout);
        
        
        if (FD_ISSET(s, &set)) 
        {
			n=recvfrom(s, &recvCommand,sizeof(int),0,(struct sockaddr *)&cli_addr,&clilen);
            printf("recv command from manager %d\r\n", recvCommand);
            //performCommandFromManager(recvCommand);
        }
		    

    }
}
