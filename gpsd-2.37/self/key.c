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
 
int main (int argc, char *argv[])
{
  struct input_event ev[64];
  int fd, rd, value, size = sizeof (struct input_event);
  char name[256] = "Unknown";
  char *device = NULL;
  int i = 0;
  static int prevValue[64] = {0};
struct input_id deviceId;
 
  //Setup check
  if (argv[1] == NULL){
      printf("Please specify (on the command line) the path to the dev event interface devicen");
      exit (0);
    }
 
  if ((getuid ()) != 0)
    printf ("You are not root! This may not work...n");
 
  if (argc > 1)
    device = argv[1];
 
  //Open Device
  if ((fd = open (device, O_RDONLY)) == -1)
    printf ("%s is not a vaild device.n", device);
 
printf("111\r\n");
  //Print Device Name
  ioctl (fd, EVIOCGNAME (sizeof (name)), name);
  printf ("Reading From : %s (%s)n", device, name);
//  ioctl (fd, EVIOCGID (sizeof (name)), name);
//  printf ("Reading device id From : %s (%s)n", device, name);
//  ioctl (fd, EVIOCGKEYCODE (sizeof (name)), name);
//  printf ("Reading key code id From : %s (%s)n", device, name);
  ioctl (fd, EVIOCGPHYS (sizeof (name)), name);
  printf ("Reading physical location id From : %s (%s)n", device, name);
  ioctl (fd, EVIOCGUNIQ (sizeof (name)), name);
  printf ("Reading unique identifier id From : %s (%s)n", device, name);


  ioctl(fd, EVIOCGID, &deviceId);

printf("-----vendor %04hx product %04hx version %04hx\r\n", deviceId.vendor, deviceId.product, deviceId.version);

printf("112\r\n");
  while (1)
  {
//      if ((rd = read (fd, ev, size * 64)) < size)
//          perror_exit ("read()");      
      rd = read(fd, (char *)&ev, sizeof(struct input_event)*64);
      if(rd > 0)
      {
        if(ev[1].value == 1 && ev[1].type == 1 && value!=0)
        {
          printf(" **** %d ****\r\n", ev[1].code);
          fflush(stdout);

        }
        //printf("ev[1].value = %d ev[1].code = %u ev[1].type \r\n", ev[1].value, ev[1].code, ev[1].type);
        for(i = 0; i < 64; i++)
        {
          if(prevValue[i] != ev[i].value)
          {
            printf("ev[%d].value = %d ev[%d].code = %u ev[%d].type \r\n", 
              i, ev[i].value, i, ev[i].code, i, ev[i].type);
          }
        }

          fflush(stdout);
          for(i = 0; i < 64; i++)
          {
            prevValue[i] = ev[i].value;
          }
/*
          for(i = 0; i < 64; i++)
          {
              printf("ev[%d].type = %u code = %u value = %d\r\n", i, ev[i].type, ev[i].code, ev[i].value);
          }
          fflush(stdout);
*/
      }
/* 
      value = ev[0].value;
 
      //if (value != ' ' && ev[1].value == 1 && ev[1].type == 1)
      //{ // Only read the key press event
       printf ("Code[%d]n", (ev[1].code));
      //}
*/
  }
 
  return 0;
} 
