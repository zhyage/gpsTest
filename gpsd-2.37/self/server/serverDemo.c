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


typedef int BOOL;
typedef char            S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
typedef long            S64;
typedef unsigned long   U64;


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
    U8  *checkSum;    
}uploadData_t;
*/