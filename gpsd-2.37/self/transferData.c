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
#include "gpsTest.h"
#include "arrQueue.h"
#include "position.h"
#include "dllist.h"
#include "transferData.h"
#include "position.h"


reportListRegister_t reportListArr[REPORT_END] = {NULL, NULL};

void registerReportList(int reportType, DLLIST **list, void *func)
{
	reportListArr[reportType].list = list;
	reportListArr[reportType].sendFunc = func;
	
	return;
}

void* transferData()
{
	
	static struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	struct timeval timeout;
	fd_set set;
	int j=0;
	reportSendNotic_t reportNotic;
	int n=0;
	int i = 0;
	int s = 0;
	int sock_opt = 1;
	
	reportNotic.reportType = -1;

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
	serv_addr.sin_port = htons(9998);
	




	if (bind (s, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
	{
		printf("error to binding 9998\n");
		close(s);
		return -1;
	}


	for(;;)
	{
		FD_ZERO(&set);
		FD_SET(s,&set);
		timeout.tv_sec=100;
		timeout.tv_usec=0;

		clilen=sizeof(struct sockaddr);

		if (j=(select(FD_SETSIZE,&set,NULL,NULL,&timeout))>0)
		{
			n=recvfrom(s, &reportNotic,sizeof(reportSendNotic_t),0,(struct sockaddr *)&cli_addr,&clilen);
				
		}
#if 1 
		printf("get notic  = %d\r\n", reportNotic.reportType);
		if(NULL == (reportListArr[reportNotic.reportType].list))
		{
			printf("no such report type\r\n");
		}
		else
		{
			reportListArr[reportNotic.reportType].sendFunc((reportListArr[reportNotic.reportType].list));	
			//DLWalk((*(reportListArr[reportNotic.reportType])), WalkPositionReport, NULL);
		}
#endif
	}

}


void sendReportNotic(reportSendNotic_t *notic )
{
		static int sockfd=0;
		struct sockaddr_in servaddr;
		char msg[20]="radiusd restart";

		
		bzero(&servaddr,sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(9998);
		
		inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
		sockfd=socket(AF_INET,SOCK_DGRAM,0);

		sendto(sockfd, notic,sizeof(reportSendNotic_t),
			0,(struct sockaddr *)&servaddr,sizeof(struct sockaddr));
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



