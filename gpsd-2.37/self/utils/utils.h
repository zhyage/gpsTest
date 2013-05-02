#ifndef UTILS_H
#define UTILS_H
void getDDMMSSSS(float x, unsigned char *DD, unsigned char *MM, unsigned short *SSSS);
double radian(double d);
double get_distance(double lat1, double lng1, double lat2, double lng2);
double get_distance_in(double lat1, double lng1, double lat2, double lng2);
double get_distance_out(double lat1, double lng1, double lat2, double lng2);

#endif
