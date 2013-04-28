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
#include "manager.h"
#include "internetConnectCheck.h"

int mainCommand = 0;

void getMainCommand()
{
    
}


void disPatchCommand(int command, int port)
{
		static int sockfd=0;
		struct sockaddr_in servaddr;

		
		bzero(&servaddr,sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(port);
		
		inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
		sockfd=socket(AF_INET,SOCK_DGRAM,0);

		sendto(sockfd, &command,sizeof(int),
			0,(struct sockaddr *)&servaddr,sizeof(struct sockaddr));
}

int main()
{

    int period = 100000;//0.1 sec
    //unsigned long count = 0;
    struct timeval timeout;
	fd_set set;
    char command[12];
    int i = 0;
    pthread_t internetConnectCheck_id;

    pthread_create(&internetConnectCheck_id, NULL, internetConnectCheck, NULL);


    for(;;)
	{
		FD_ZERO(&set);
		FD_SET(0,&set);
		timeout.tv_sec=0;
		timeout.tv_usec=100000;
        memset(command, 0, 12);
   
       
        select(FD_SETSIZE,&set,NULL,NULL,&timeout);
		

        if (FD_ISSET(0, &set)) 
        {
            //command = gets(stdin);
            if (fgets(command, LINE_MAX, stdin) != NULL) 
            {
                if('6' == command[0])
                {
                    printf("next stop\r\n");
                    disPatchCommand(NEXT_STOP_ANNOUNCE, PORT_ANNOUNCE);
                }
                else if('4' == command[0])
                {
                    printf("prev stop\r\n");
                    disPatchCommand(PREV_STOP_ANNOUNCE, PORT_ANNOUNCE);
                }
                else
                {
                    printf("unknow command\r\n");
                }
            }
            
        }     
        
    }
}