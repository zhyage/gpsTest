#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/utils.h"

int main()
{
  double lat_1 = 30.277810;
  double lng_1 = 120.332080;

  double lat_2 = 30.278160;
  double lng_2 = 120.332430;

//        {VALID, 120.332080,    30.277810,  ADD,    ADD,    "stop1_in.mp3",     "stop1_out.mp3"},
//        {VALID, 120.332430,    30.278160,  ADD,    ADD,    "huancheng.mp3",     NULL},
  printf("distance = %f \r\n", get_distance(lat_1, lng_1, lat_2, lng_2));

  lat_1 = 30.295750;
  lng_1 = 120.385487;

lat_2 = 30.279267;
lng_2 = 120.377392;
//        {VALID, 120.385487,    30.295750 ,  UNKNOW,    REDUCE,    "12_road_in.mp3",     "12_road_out.mp3"},
//        {VALID, 120.377392,    30.279267  ,  UNKNOW,    REDUCE,    "20_road_in.mp3",     "20_road_out.mp3"},
  printf("distance = %f \r\n", get_distance(lat_1, lng_1, lat_2, lng_2));


  lat_1 = 30.295750;
  lng_1 = 120.385487;

lat_2 = 30.295820;
lng_2 = 120.385557;
//        {VALID, 120.385487,    30.295750 ,  UNKNOW,    REDUCE,    "12_road_in.mp3",     "12_road_out.mp3"},
//        {VALID, 120.377392,    30.279267  ,  UNKNOW,    REDUCE,    "20_road_in.mp3",     "20_road_out.mp3"},
  printf("distance = %f \r\n", get_distance(lat_1, lng_1, lat_2, lng_2));

}
