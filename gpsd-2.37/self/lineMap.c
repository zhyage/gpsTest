#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lineMap.h"

/*must index with lineId */
lineData_t lineData[] = 
{
	{INVALID_ID,	NULL,	{0}},
    {1, "zhi_jiang_dong_lu",       {8, 9, 10, 11, 12}},
    {2, "yan_jiang_da_dao", {1, 2, 6, 3, 4, 5, 7}},
};

#if 0
allLineMark_t cityData[] = 
{
    {0, "xiasha", NULL},
    {INVALID_ID, NULL, NULL}
};
#endif

unsigned int getLineNum()
{
	return sizeof(lineData)/sizeof(lineData_t);
}

lineData_t *getLineData(unsigned int lineId)
{
	int lineNum = getLineNum();
//	printf("totally have %d lines\r\n", lineNum);
	if(0 == lineId || lineId > lineNum)
	{
		printf("invalid lineId\r\n");
		return NULL;
	}
	return &lineData[lineId];
}



/*must index with stopId */
busStopMark_t allBusStop[] = 
{
	{
        INVALID_ID,
        NULL,
        STOP,
        {INVALID, 0,    0,  UNKNOW,    UNKNOW,    NULL,     NULL},
        {INVALID, 0,    0,  UNKNOW,    UNKNOW,    NULL,     NULL},
    },
    {
        1,
        "stop1",
        STOP,
        {VALID, 120.332080,    30.277810,  ADD,    ADD,    "stop1_in.mp3",     "stop1_out.mp3"},
        {VALID, 120.332087,    30.277817,  REDUCE, REDUCE,    "stop1_in.mp3",     "stop1_out.mp3"},
    },
    {
        2,
        "huancheng",
        CROOKED,
        {VALID, 120.332430,    30.278160,  ADD,    ADD,    "huancheng.mp3",     NULL},
        {INVALID, 0,    0,  REDUCE,    REDUCE,    "stop2_in.mp3",     "stop2_out.mp3"},
    },
    {
        3,
        "stop2",
        STOP,
        {VALID, 120.332430,    30.278160,  ADD,    ADD,    "stop2_in.mp3",     "stop2_out.mp3"},
        {VALID, 120.332437,    30.278167,  REDUCE,    REDUCE,    "stop2_in.mp3",     "stop2_out.mp3"},
    },
    {
        4,
        "stop3",
        STOP,
        {VALID, 120.332780,    30.278510,  ADD,    ADD,    "stop3_in.mp3",     "stop3_out.mp3"},
        {VALID, 120.332787,    30.278517,  REDUCE,    REDUCE,    "stop3_in.mp3",     "stop3_out.mp3"},
    },
    {
        5,
        "stop4",
        STOP,
        {VALID, 120.333130,    30.278860,  ADD,    ADD,    "stop4_in.mp3",     "stop4_out.mp3"},
        {VALID, 120.333137,    30.278867,  REDUCE,    REDUCE,    "stop4_in.mp3",     "stop4_out.mp3"},
    },
    {
        6,
        "shopping",
        HOTSPOT,
        {INVALID, 120.332430,    30.278160,  ADD,    ADD,    "shopping.mp3",     NULL},
        {VALID, 120.332430,    30.278160,  REDUCE,    REDUCE,    "shopping.mp3",     NULL},
    },
    {
        7,
        "end",
        STOP,
        {VALID, 120.333130,    30.278860,  ADD,    ADD,    "end.mp3",     NULL},
        {INVALID, 120.333137,    30.278867,  REDUCE,    REDUCE,    "stop4_in.mp3",     "stop4_out.mp3"},
    },
    {
        8,
        "12-road",
        STOP,
        {VALID, 120.385487,    30.295750 ,  UNKNOW,    REDUCE,    "12_road_in.mp3",     "12_road_out.mp3"},
        {VALID, 120.387827,    30.300171,  UNKNOW,    ADD,    "12_road_in.mp3",     "12_road_out.mp3"},
    },
    {
        9,
        "20-road",
        STOP,
        {VALID, 120.377392,    30.279267  ,  UNKNOW,    REDUCE,    "20_road_in.mp3",     "20_road_out.mp3"},
        {VALID, 120.377296,    30.278759,  UNKNOW,    ADD,    "20_road_in.mp3",     "20_road_out.mp3"},
    },
    {
        10,
        "19-road",
        STOP,
        {VALID, 120.361372,    30.266597,  REDUCE,    UNKNOW,    "20_road_in.mp3",     "20_road_out.mp3"},
        {VALID, 120.360810,    30.266480,  ADD,    UNKNOW,    "20_road_in.mp3",     "20_road_out.mp3"},
    },
    {
        11,
        "bridge-road",
        STOP,
        {VALID, 120.353790,    30.266574,  REDUCE,    UNKNOW,    "bridge_road_in.mp3",     "bridge_road_out.mp3"},
        {VALID, 120.353245,    30.266366,  ADD,    UNKNOW,    "bridge_road_in.mp3",     "bridge_road_out.mp3"},
    },
    {
        12,
        "fengfan-road",
        STOP,
        {VALID, 120.338672,    30.270879,  REDUCE,    UNKNOW,    "fengfan_road_in.mp3",     "fengfan_road_out.mp3"},
        {VALID, 120.338009,    30.271021,  ADD,    UNKNOW,    "fengfan_road_in.mp3",     "fengfan_road_out.mp3"},
    },
    {
        INVALID_ID,
        NULL,
        STOP,
        {INVALID, 0,    0,  UNKNOW,    UNKNOW,    NULL,     NULL},
        {INVALID, 0,    0,  UNKNOW,    UNKNOW,    NULL,     NULL},
    },
    
};

int getStopIdOfLine(int lineId, int num)
{
    int stopId = 0;
    lineData_t *lineData = getLineData(lineId);
    if(NULL == lineData)
    {
    	return -1;
    }
    stopId = lineData->stopId[num];
    if(0 == stopId)//invalide stopId
    {
    	return -1;
    }
    return stopId;
}

int getNextStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *nextStop)
{
    int i = 0;
    int stopId = 0;
    unsigned nextStopId = 0;
    int upOrDown = UPLINE;

    //printf("get next lineId = %d\r\n", lineId);
    lineData_t *lineData = getLineData(lineId);
    if(NULL == lineData)
    {
    	return -1;
    }

    for(i = 0; i < 256; i++)
    {
    	if(curStopId == lineData->stopId[i])
    	{
    		nextStopId = lineData->stopId[i + 1];
    		if(0 == nextStopId)//the last stop, need turn around
    		{
    			nextStopId = lineData->stopId[i - 1];
    			if(nextStopId == 0)//can be 0
    			{
    				return -1;
    			}
    			upOrDown = 0 - curUpOrDown;
    			nextStop->stopId = nextStopId;
    			nextStop->upOrDown = upOrDown;
    			return nextStopId;
    		}
    		else
    		{
    			upOrDown = curUpOrDown;
    			nextStop->stopId = nextStopId;
    			nextStop->upOrDown = upOrDown;
    			return nextStopId;

    		}
    	}

    }
    return -1;//no this stopid in this line
    
}

int getPrevStop(int curStopId, int curUpOrDown, int lineId, stopPend_t *prevStop)
{
    int i = 0;
    int stopId = 0;
    unsigned prevStopId = 0;
    int upOrDown = UPLINE;

    lineData_t *lineData = getLineData(lineId);
    if(NULL == lineData)
    {
    	return -1;
    }

    for(i = 0; i < 256; i++)
    {
    	if(curStopId == lineData->stopId[i])
    	{
    		prevStopId = lineData->stopId[i - 1];
    		if(0 == prevStopId)//the first stop, need turn around
    		{
    			prevStopId = lineData->stopId[i + 1];
    			if(prevStopId == 0)//can be 0
    			{
    				return -1;
    			}
    			upOrDown = 0 - curUpOrDown;
    			prevStop->stopId = prevStopId;
    			prevStop->upOrDown = upOrDown;
    			return prevStopId;
    		}
    		else
    		{
    			upOrDown = curUpOrDown;
    			prevStop->stopId = prevStopId;
    			prevStop->upOrDown = upOrDown;
    			return prevStopId;

    		}
    	}

    }
    return -1;//no this stopid in this line
    
}

busStopMark_t *getBusStopBystopId(unsigned short stopId)
{
    return &allBusStop[stopId];
}
