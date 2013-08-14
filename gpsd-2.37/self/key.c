#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include "manager.h"
 
/*
button 0 : PREV_STOP_ANNOUNCE
button 1 : NEXT_STOP_ANNOUNCE
button 2: START_MPLAYER
button 3: STOP_MPLAYER
button 4: SWITCH_DVR_CHANNEL
button 5: MAKE_CALL
*/ 

const char *startMplayerCommandLine = "/self_bin/mplayer/mplayer rtsp://admin:acamar@192.168.3.3/mpeg4/ch1/main/av_stream </dev/null >/dev/null 2>&1 &";
const char *startMplayerCommandLineChannel2 = "/self_bin/mplayer/mplayer rtsp://admin:acamar@192.168.3.3/mpeg4/ch2/main/av_stream </dev/null >/dev/null 2>&1 &";
const char *startMplayerCommandLineChannel3 = "/self_bin/mplayer/mplayer rtsp://admin:acamar@192.168.3.3/mpeg4/ch3/main/av_stream </dev/null >/dev/null 2>&1 &";
const char *startMplayerCommandLineChannel4 = "/self_bin/mplayer/mplayer rtsp://admin:acamar@192.168.3.3/mpeg4/ch4/main/av_stream </dev/null >/dev/null 2>&1 &";
const char *stopMplayerCommandLine = "killall mplayer";
 
void* keyBoardCommand ()
{

    int buttons_fd;
    char buttons[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};
    int current_dvr_channel = 1;

    buttons_fd = open("/dev/buttons", 0);
    if (buttons_fd < 0) 
    {
        printf("error to open device buttons\r\n");
        exit(1);
    }


    for (;;) 
    {
        char current_buttons[8];
        int count_of_changed_key;
        int i;
        if (read(buttons_fd, current_buttons, sizeof(current_buttons)) != sizeof(current_buttons)) 
        {
            printf("error to read buttons:");
            exit(1);
        }


        for (i = 0, count_of_changed_key = 0; i < sizeof(buttons) / sizeof (buttons[0]); i++) 
        {
            if (buttons[i] != current_buttons[i]) 
            {
                //buttons[i] = current_buttons[i];
                if(buttons[i] == '0')
                {
                    printf("iiiiiiiiiiiii = %d\r\n", i);
                    switch (i)
                    {
                    case 0:
                        {
                            printf("button : PREV_STOP_ANNOUNCE\r\n");
                            //disPatchCommand(PREV_STOP_ANNOUNCE, PORT_ANNOUNCE);
                        }
                        break;
                    case 1:
                        {
                            printf("button : NEXT_STOP_ANNOUNCE\r\n");
                            //disPatchCommand(NEXT_STOP_ANNOUNCE, PORT_ANNOUNCE);
                        }
                        break;
                    case 2:
                        {
                            printf("button : START_MPLAYER\r\n");
                            system(stopMplayerCommandLine);
                            system(startMplayerCommandLine);
                            //sleep(10);
                        }
                        break;
                    case 3:
                        {
                            printf("button : STOP_MPLAYER\r\n");
                            system(stopMplayerCommandLine);
                        }
                        break;
                    case 4:
                        {
                            printf("button : SWITCH_DVR_CHANNEL current channel = %d\r\n", current_dvr_channel);
                            if(current_dvr_channel >= 4)
                            {
                                current_dvr_channel = 1;
                            }
                            else
                            {
                                current_dvr_channel = current_dvr_channel + 1;
                            }
                            system(stopMplayerCommandLine);
                            if(current_dvr_channel == 1)
                            {
                                printf("mplayer commandline = %s\r\n", startMplayerCommandLine);
                                system(startMplayerCommandLine);
                                //sleep(10);
                            }
                            if(current_dvr_channel == 2)
                            {
                                printf("mplayer commandline = %s\r\n", startMplayerCommandLineChannel2);
                                system(startMplayerCommandLineChannel2);
                                //sleep(10);
                            }
                            if(current_dvr_channel == 3)
                            {
                                printf("mplayer commandline = %s\r\n", startMplayerCommandLineChannel3);
                                system(startMplayerCommandLineChannel3);
                                //sleep(10);
                            }
                            if(current_dvr_channel == 4)
                            {
                                printf("mplayer commandline = %s\r\n", startMplayerCommandLineChannel4);
                                system(startMplayerCommandLineChannel4);
                                //sleep(10);
                            }
                        }
                        break;
                    case 5:
                        {
                            printf("button : MAKE_CALL\r\n");
                        }
                        break;
                    default:
                        {
                            printf("button : unknow button\r\n");
                        }
                        break;
                    }
                }
                buttons[i] = current_buttons[i];
            }
        }

    }

    close(buttons_fd);
  return 0;
} 
