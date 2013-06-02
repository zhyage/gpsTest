insmod /opt/self_module/gps/cdc-acm.ko
 mknod -m 666 /dev/ttyACM c 166 0
 gpsd /dev/ttyACM
