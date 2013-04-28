#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#if 1
#include "internetConnectCheck.h"
#include "manager.h"

internetConnectStatus_t g_internetConnectStatus = {DISCONNECTED, DISCONNECTED};

void setConnectStatus(int status)
{
	g_internetConnectStatus.prevStatus = g_internetConnectStatus.currStatus;
	g_internetConnectStatus.currStatus = status;
}

void* internetConnectCheck()
{
	for(;;)
	{
		int status = DISCONNECTED;
		sleep(1);
		status = checkInternetConnect();
		setConnectStatus(status);
		if(g_internetConnectStatus.currStatus == CONNECTED 
			&& g_internetConnectStatus.prevStatus == DISCONNECTED)
		{
			printf("start hand shake\r\n");
			disPatchCommand(INTERNET_CONNECT_HAND_SHAKE, PORT_ANNOUNCE);
		}

	}

}
#endif

#if 0
int main()
{
	for(;;)
	{
		sleep(1);
		checkInternetConnect();
	}
}
#endif


int checkInternetConnect()
{
	CURL *curl;
	CURLcode res;
	int connect = -1;

	curl = curl_easy_init();
	if(curl) 
	{
  		curl_easy_setopt(curl, CURLOPT_URL, "111.13.47.157");
  		res = curl_easy_perform(curl);
  		if(CURLE_OK != res)
  		{
  			switch(res)
  			{
  				case CURLE_COULDNT_CONNECT:
				case CURLE_COULDNT_RESOLVE_HOST:
		     	case CURLE_COULDNT_RESOLVE_PROXY:
		     	{
		     		printf("internet dose not exist\r\n");
		     		connect = -1;		     		
		     	}
		     	break;
  			}
  		}
  		else
  		{
  			printf("internet exist\r\n");
  			connect = 1 ;
  		}
  		/* always cleanup */ 
  		
	}
	curl_easy_cleanup(curl);
	return connect;
}