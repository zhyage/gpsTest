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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gpsTest.h"
#include "manager.h"
#include "internetConnectCheck.h"
#include "recvSession.h"
#include "key.h"

int mainCommand = 0;

void getMainCommand()
{
    
}


void disPatchCommand(int command, int port)
{
		static int sockfd=0;
        struct sockaddr_in servaddr;
        //if(0 == sockfd)
        //{
    		servaddr.sin_family = AF_INET;
    		servaddr.sin_port = htons(port);
    		
    		inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
    		sockfd=socket(AF_INET,SOCK_DGRAM,0);
        //}
		sendto(sockfd, &command,sizeof(int),
			0,(struct sockaddr *)&servaddr,sizeof(struct sockaddr));
}

int handleRemoteCommand(unsigned char *data, unsigned short dataLen)
{
    pushCommandData_t recvData;
    return recvFromRemote(data, dataLen, &recvData);
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
    pthread_t keyBoardCommand_id;
    int s = 0;
    int sock_opt = 1;
    static struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    char remotePushCommand[128];

//    pthread_create(&internetConnectCheck_id, NULL, internetConnectCheck, NULL);
    pthread_create(&keyBoardCommand_id, NULL, keyBoardCommand, NULL);
    
  
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
    serv_addr.sin_port = htons(9998);
    

    if (bind (s, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("error to binding 9998\n");
        close(s);
        return -1;
    }


    for(;;)
	{
        int n = 0;
		FD_ZERO(&set);
//		FD_SET(0,&set);
        FD_SET(s, &set);
		//timeout.tv_sec=0;
		//timeout.tv_usec=100000;
		timeout.tv_sec=2;
		timeout.tv_usec=0;
        memset(command, 0, 12);
        memset(remotePushCommand, 0, 128);
   
       
        select(FD_SETSIZE,&set,NULL,NULL,&timeout);
		
/*
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
        else
*/        
        {
            int fd = 0;
            int len = 0;
            char scpCmd[128];
            struct stat newStat;
            struct stat oldStat;
            static int mostTime = 0;
            time_t time1;
            time_t time2;

            memset(scpCmd, 0, 128);

            time1 = time(NULL);
            //sprintf(scpCmd, "%s", "sshpass -p 'acamar' scp -p root@127.0.0.1:/opt/set_line /opt/oper_on .");
            //sprintf(scpCmd, "%s", "sshpass -p 'yqj810828' scp -p -P 2224 yqj@111.13.47.157:/var/www/command/test1/set_line .");
            //sprintf(scpCmd, "%s", "sshpass -p 'yqj810828' scp -p -P 2224 yqj@111.13.47.157:/var/www/command/test1/* .");
            sprintf(scpCmd, "%s", "curl -m 2 -R -O --remote-name http://111.13.47.154:8090/html/test1/switch_status");
            printf("scpCmd = %s\r\n", scpCmd);
            system(scpCmd);
            sprintf(scpCmd, "%s", "curl -m 2 -R -O --remote-name http://111.13.47.154:8090/html/test1/set_route");
            printf("scpCmd = %s\r\n", scpCmd);
            system(scpCmd);
/*            
            memset(scpCmd, 0, 128);
            sprintf(scpCmd, "%s", "sshpass -p 'yqj810828' scp -p -P 2224 yqj@111.13.47.157:/var/www/command/test1/oper_on .");
            printf("scpCmd = %s\r\n", scpCmd);
            system(scpCmd);
*/            
            time2 = time(NULL);
            
            printf("using %d sec to get file\r\n", time2-time1);
            if(time2-time1 > mostTime)
            {
                mostTime = time2-time1;
            }
            printf("the most long time = %d \r\n", mostTime);
            
            
            if(0 == stat("./set_route", &newStat) && 0 == stat("./set_route.old", &oldStat))
            {
            /*
                printf("time stamp of new = %ld %ld %ld\r\n", 
                    newStat.st_atime,
                    newStat.st_mtime,
                    newStat.st_ctime
                    );
                    
                printf("time stamp of old = %ld %ld %ld\r\n", 
                    oldStat.st_atime,
                    oldStat.st_mtime,
                    oldStat.st_ctime
                    );    
            */
                if(newStat.st_mtime != oldStat.st_mtime)//new file
                {
                    printf("need to do handle of set_route\r\n");
                    fd = open("./set_route", O_RDONLY);
                    len = read(fd, remotePushCommand, sizeof(remotePushCommand));

                    printf("recv remote push command data length =  %d\r\n", len);
                    handleRemoteCommand(remotePushCommand, len);
                    printf("end of handle push command\r\n");
                }
            }
            if(0 == stat("./switch_status", &newStat) && 0 == stat("./switch_status.old", &oldStat))
            {
                if(newStat.st_mtime != oldStat.st_mtime)//new file
                {
                    printf("need to do handle of switch_status\r\n");
                    fd = open("./switch_status", O_RDONLY);
                    len = read(fd, remotePushCommand, sizeof(remotePushCommand));

                    printf("recv remote push command data length =  %d\r\n", len);
                    handleRemoteCommand(remotePushCommand, len);
                    printf("end of handle push command\r\n");
                }
            }
            /*
            if(0 == stat("./oper_on", &newStat))
            {
                printf("after time stamp of new = %ld %ld %ld\r\n", 
                    newStat.st_atime,
                    newStat.st_mtime,
                    newStat.st_ctime
                    );
            }
            */
            system("mv ./set_route ./set_route.old");
            system("mv ./switch_status ./switch_status.old");
            /*
            if(0 == stat("./oper_on.old", &oldStat))
            {
                
                    
                printf("after time stamp of old = %ld %ld %ld\r\n", 
                    oldStat.st_atime,
                    oldStat.st_mtime,
                    oldStat.st_ctime
                    ); 
            }
            */

            
            
            /*

            printf("timeout to get push command\r\n");
            //fd = open("set_line", O_RDONLY);
            fd = open("oper_on", O_RDONLY);
            len = read(fd, remotePushCommand, sizeof(remotePushCommand));

            printf("recv remote push command data length =  %d\r\n", len);
            handleRemoteCommand(remotePushCommand, len);
            printf("end of handle push command\r\n");
            */

        }

        /*
        if(FD_ISSET(s, &set))
        {
            n=recvfrom(s, remotePushCommand, 128, 0, (struct sockaddr *)&cli_addr, &clilen);
            printf("recv remote push command data length =  %d\r\n", n);
            handleRemoteCommand(remotePushCommand, n);
        } 
        */
        
    }
}
