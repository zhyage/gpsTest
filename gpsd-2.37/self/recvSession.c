#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "recvSession.h"

int handleScheduleLineCmd(unsigned char *data, unsigned short length)
{
	scheduleLineCommand_t scheduleCmd;
	unsigned char *pos = data;
  int lineId = 0;
  char cmd[64];
  memset(cmd, 0, 64);
	memset(&scheduleCmd, 0, sizeof(scheduleLineCommand_t));
	strcpy(scheduleCmd.lineName, pos);
	pos += strlen(scheduleCmd.lineName) + 1;
	memcpy(&scheduleCmd.lineId, pos, sizeof(scheduleCmd.lineId));


	printf("handleScheduleLineCmd, lineName:%s lineId :%d:%d:%d\r\n", 
		scheduleCmd.lineName, 
		scheduleCmd.lineId[0], scheduleCmd.lineId[1], scheduleCmd.lineId[2]);
/*
  pos = (unsigned char *)&lineId;
  pos[1] = scheduleCmd.lineId[0];
  pos[2] = scheduleCmd.lineId[1];
  pos[3] = scheduleCmd.lineId[2];
*/
  lineId = scheduleCmd.lineId[0] * 256 *256 + scheduleCmd.lineId[1] * 256 + scheduleCmd.lineId[2];

  sprintf(cmd, "%s", "killall gpsTest");
  system(cmd);
  sleep(5);

//  sprintf(cmd, "%s", "./gpsTest -l %d &", lineId);
  sprintf(cmd, "./gpsTest -l %d &", lineId);
  printf("restart gpsTest cmd = %s\r\n", cmd);

  system(cmd);

	return 1;

}

int handleInOutCmd(unsigned char *data, unsigned short length)
{
	inOutCommand_t inOutCmd;
    char cmd[64];
    memset(cmd, 0, 64);
	memcpy(&inOutCmd, data, sizeof(inOutCommand_t));

	printf("handleInOutCmd inOrOut = %d confirm = %d\r\n", 
		inOutCmd.inOrOut, inOutCmd.confirm);
    if(inOutCmd.inOrOut == 0x01)//out
    {
        sprintf(cmd, "%s", "killall gpsTest");
        system(cmd);
        sleep(5);
    }
    if(inOutCmd.inOrOut == 0x00)//in
    {
        sprintf(cmd, "%s", "killall gpsTest");
        system(cmd);
        sleep(5);

        sprintf(cmd, "%s", "./gpsTest -l 1 &");
        system(cmd);
    }
	return 1;
}


int recvFromRemote(unsigned char *data, unsigned short dataLength, 
	pushCommandData_t *recvData)
{
	unsigned char *pos = data;
	unsigned short strlength = 0;
	unsigned short countLength = 0;
	unsigned short subDataLength = 0;

	int i = 0;

	if(dataLength <= 4)
	{
		printf("incorrect remoteData\r\n");
		return -1;
	}

	memcpy(&recvData->startTag, pos, sizeof(recvData->startTag));
	pos += sizeof(recvData->startTag);
    recvData->startTag = htons(recvData->startTag);
//    recvData->startTag = (recvData->startTag);


	memcpy(&recvData->length, pos, sizeof(recvData->length));
	pos += sizeof(recvData->length);
    recvData->length = htons(recvData->length);
//    recvData->length = (recvData->length);

	memcpy(&recvData->version, pos, sizeof(recvData->version));
	pos += sizeof(recvData->version);
	countLength += sizeof(recvData->version);

	memcpy(&recvData->sessionId, pos, sizeof(recvData->sessionId));
	pos += sizeof(recvData->sessionId);
	countLength += sizeof(recvData->sessionId);

/*
	strlength = 0;
	while((pos[i] != '0x00') && (i <= 14))
	{
		i = i + 1;
	}
	strlength = i + 2;//which '\0'

	printf("strlength = %d\r\n", strlength);

	memcpy(recvData->motoId, pos, strlength);
	pos += strlength;
	countLength += strlength;
*/

	strlength = strlen(pos);
	strcpy(recvData->motoId, pos);
	pos += strlength + 1;
	countLength += strlength + 1;

	memcpy(&recvData->date, pos, sizeof(recvData->date));
	pos += sizeof(recvData->date);
	countLength += sizeof(recvData->date);

	memcpy(&recvData->time, pos, sizeof(recvData->time));
	pos += sizeof(recvData->time);
	countLength += sizeof(recvData->time);

	memcpy(&recvData->commandId, pos, sizeof(recvData->commandId));
	pos += sizeof(recvData->commandId);
	countLength += sizeof(recvData->commandId);

	subDataLength = recvData->length - countLength;
	printf("subDataLength = %d recvData->length = %d countLength = %d\r\n"
		, subDataLength, recvData->length, countLength);

	memcpy(recvData->data, pos, subDataLength);
	pos += subDataLength;

	memcpy(&recvData->checkSum, pos, sizeof(recvData->checkSum));
	pos += sizeof(recvData->checkSum);

	printf("recvData->commandId = %02x\r\n", recvData->commandId);
	switch (recvData->commandId)
	{
		case COMMAND_TEXT_INFO_PUSH:
		{
			printf("get COMMAND_TEXT_INFO_PUSH\r\n");
		}
		break;
		case COMMAND_SCHEDULE_LINE_PUSH:
		{
			printf("get COMMAND_SCHEDULE_LINE_PUSH\r\n");
			return handleScheduleLineCmd(recvData->data, subDataLength);
		}
		break;
		case COMMAND_IN_OUT_PUSH:
		{
			printf("get COMMAND_IN_OUT_PUSH\r\n");
			return handleInOutCmd(recvData->data, subDataLength);
		}
		break;
		default:
		{
			printf("get UNKNOW remote push command\r\n");
		}	
		break;
	}

}
