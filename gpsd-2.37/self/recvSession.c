#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "recvSession.h"




int recvFromRemote(unsigned char *data, unsigned short dataLength, pushCommandData_t *recvData)
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


	memcpy(&recvData->length, pos, sizeof(recvData->length));
	pos += sizeof(recvData->length);

	memcpy(&recvData->version, pos, sizeof(recvData->version));
	pos += sizeof(recvData->version);
	countLength += sizeof(recvData->version);

	memcpy(&recvData->sessionId, pos, sizeof(recvData->sessionId));
	pos += sizeof(recvData->sessionId);
	countLength += sizeof(recvData->version);

	strlength = 0;
	while((pos[i] != '0x00') && (i <= 14))
	{
		i = i + 1;
	}
	strlength = i + 2;//which '\0'

	memcpy(recvData->motoId, pos, strlength);
	pos += sizeof(strlength);
	countLength += strlength;

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

	memcpy(recvData->data, pos, subDataLength);
	pos += subDataLength;

	memcpy(&recvData->checkSum, pos, sizeof(recvData->checkSum));
	pos += sizeof(recvData->checkSum);


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
		}
		break;
		case COMMAND_IN_OUT_PUSH:
		{
			printf("get COMMAND_IN_OUT_PUSH\r\n");
		}
		break;
	}

}
