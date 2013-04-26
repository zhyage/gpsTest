#ifndef IDENTITY_H
#define IDENTITY_H

unsigned char *getMotoId();
unsigned char *getSoftVersion();
unsigned char *getDeviceId();
unsigned char *getMotoLicense();
unsigned char *getSIMId();
unsigned char *getBasePosition();
unsigned char *getBaseCell();
unsigned int getLineId();
void setLineId(unsigned int lineId);


#endif