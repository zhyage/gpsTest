#include <stdio.h>
#include <string.h>
#include <stdlib.h>


unsigned char *g_motoId = "bus 0001";

unsigned char *g_softVersion = "0.0001";

unsigned char *g_deviceId = "00001";

unsigned char *g_motoLicense = "A7680G";

unsigned char *g_SIMId = "abcdefghijklmnopqrst";

unsigned char *g_basePosition = "3.3, 6.6";

unsigned char *g_baseCell = "6-6";

unsigned int g_lineId = 2;

unsigned char *g_driverId = "driver-001";

unsigned char *getMotoId()
{
    return g_motoId;
}

unsigned char *getSoftVersion()
{
    return g_softVersion;
}

unsigned char *getDeviceId()
{
    return g_deviceId;
}

unsigned char *getMotoLicense()
{
    return g_motoLicense;
}

unsigned char *getSIMId()
{
    return g_SIMId;
}

unsigned char *getBasePosition()
{
	return g_basePosition;
}

unsigned char *getBaseCell()
{
	return g_baseCell;
}

unsigned int getLineId()
{
	return g_lineId;
}

void setLineId(unsigned int lineId)
{
	g_lineId = lineId;
}

unsigned char *getDriverId()
{
	return g_driverId;
}
