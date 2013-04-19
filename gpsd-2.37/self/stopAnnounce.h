#ifndef STOP_ANNOUNCE_H
#define STOP_ANNOUNCE_H

typedef struct
{
    double longitude;
    double latitude;
    double radius;
    int longitudeAttr;//+ or - or ==
    int latitudeAttr;//+ or - or ==
    char *nameOfMp3;
    
}busStopMark_t;



void* stopAnnounce();

#endif
