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
 
void handler (int sig)
{
  printf ("nexiting...(%d)n", sig);
  exit (0);
}
 
void perror_exit (char *error)
{
  perror (error);
  handler (9);
}
 
void* keyBoardCommand ()
{
  struct input_event ev[64];
  int fd, rd, value, size = sizeof (struct input_event);
  char name[256] = "Unknown";
  char *device = NULL;
  int i = 0;
  static int prevValue[64] = {0};
  struct input_id deviceId;
 
 
  if ((getuid ()) != 0)
    printf ("You are not root! This may not work...n");
 
  //Open Device
  if ((fd = open ("/dev/input/event1", O_RDONLY)) == -1)
    printf ("%s is not a vaild device.n", device);

#if 0 
  //Print Device Name
  ioctl (fd, EVIOCGNAME (sizeof (name)), name);
  printf ("Reading From : %s (%s)n", device, name);
  ioctl (fd, EVIOCGPHYS (sizeof (name)), name);
  printf ("Reading physical location id From : %s (%s)n", device, name);
  ioctl (fd, EVIOCGUNIQ (sizeof (name)), name);
  printf ("Reading unique identifier id From : %s (%s)n", device, name);
  ioctl(fd, EVIOCGID, &deviceId);
  printf("-----vendor %04hx product %04hx version %04hx\r\n", deviceId.vendor, deviceId.product, deviceId.version);
#endif

  while (1)
  {      
      rd = read(fd, (char *)&ev, sizeof(struct input_event)*64);
      if(rd > 0)
      {
        if(ev[0].value == 1 && ev[0].type == 1)
        {
          if(ev[0].code == 77)
          {
            printf(" get the key of next stop\r\n");
            disPatchCommand(NEXT_STOP_ANNOUNCE, PORT_ANNOUNCE);
      
          }
          if(ev[0].code == 75)
          {
            printf(" get the key of prev stop\r\n");
            disPatchCommand(PREV_STOP_ANNOUNCE, PORT_ANNOUNCE);
          }

        }
        fflush(stdout);
#if 0
        for(i = 0; i < 64; i++)
        {
          if(prevValue[i] != ev[i].value)
          {
            printf("ev[%d].value = %d ev[%d].code = %u ev[%d].type = %d \r\n", 
              i, ev[i].value, i, ev[i].code, i, ev[i].type);
          }
        }

          fflush(stdout);
          for(i = 0; i < 64; i++)
          {
            prevValue[i] = ev[i].value;
          }
#endif
      }
  }
 
  return 0;
} 
