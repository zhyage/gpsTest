#! /bin/sh

NGINX_DIR=/mnt/upgflash/LocalServer/Nginx
LOG_FILE=/mnt/upgflash/LocalServer/Zergling/log/trace.log

## Check nginx is up or not
ret=`ps -ef|grep -c '[n]ginx'`
if [ $ret -ne 0 ]
then
	## Do nothging since nginx is already up
	echo "$(date "+%F %T") Do nothing since nginx is already up!" >> $LOG_FILE
	exit 0
fi

if [ ! -d $NGINX_DIR ]
then
	## nginx is not installed
	echo "$(date "+%F %T") nginx is NOT installed!" >> $LOG_FILE
	exit 1
fi

## start nginx
$NGINX_DIR/sbin/nginx -p $NGINX_DIR >/dev/null 2>&1  &
echo "$(date "+%F %T") nginx is UP!" >> $LOG_FILE

exit 0
