#! /bin/sh

### ABOUT
### Runs rsync, retrying on errors up to a maximum number of tries.
### Simply edit the rsync line in the script to whatever parameters you need.

RSYNC_URL=$1
RSYNC_DEST=$2
ROOT_DIR="/opt/self_bin/Zergling"

log()
{
    MESSAGE=$1
    echo "$(date "+%F %T") $MESSAGE" >> $ROOT_DIR/log/trace.log
}

update_rsync_conf()
{
	echo "RSYNC_URL=\"$RSYNC_URL\"" > $ROOT_DIR/conf/rsync.lua 
	cat $RSYNC_DEST/version.lua >> $ROOT_DIR/conf/rsync.lua

	log "INFO: Successfully updated rsync.lua"
}


sync()
{
	SRC_URL=$1
	DEST_DIR=$2

	RSYNC="/opt/bin/rsync"
	## Check if rsync exists or not
    if [ ! -f $RSYNC ]
    then
        log "ERROR: $RSYNC does not exist." >> $LOG_FILE
        return 36
    fi

	## Clean the log file
	RSYNC_LOG=$ROOT_DIR/log/rsync.log
	> $RSYNC_LOG

    ## Usage: rsync [OPTION]... rsync://[USER@]HOST[:PORT]/SRC [DEST]
    ##
    ## Options:
    ## -a, --archive			This is equivalent to -rlptgoD. It is a quick way of saying you want 
    ## recursion and want to preserve almost everything.
    ##
    ## -v, --verbose			increase verbosity
    ## --partial				keep partially transferred files
    ## --progress				show progress during transfer
    ## -P						same as --partial --progress
    ## -z, --compress			compress file data during the transfer
    ## --delete					delete extraneous files from dest dirs
    ## --timeout=TIMEOUT		This option allows you to set a maximum I/O timeout in seconds.
    ## If no data is transferred for the specified time then rsync will exit.
    ##
    ## --contimeout=TIMEOUT		This option allows you to set the amount of time that rsync will wait for 
    ## its connection to an rsync daemon to succeed.  If the timeout is reached, rsync exits with an error.
	##

	MAX_RETRIES=20
	i=0
	err_code=0
	while [ $i -lt $MAX_RETRIES ]
	do
		i=$(($i+1))
		$RSYNC -avzP --delete --timeout=60 --contimeout=120 --log-file=$RSYNC_LOG $SRC_URL $DEST_DIR > /dev/null 2>&1
		err_code=$?
		if [ $err_code = 0 ]
		then
			log "INFO: [$DEST_DIR] was successfully synchronized with the [$SRC_URL]"

			update_rsync_conf
			return 0
		fi

		log "INFO: Tried rsync $i times"
		## Sleep 15 seconds before next sync
		sleep 15
	done

	log "ERROR: The retry limit was exceeded"
	log "ERROR: Failed to synchronize the [$DEST_DIR] with the [$SRC_URL], error code:[$err_code]"
	return $err_code
}

ret=`sync $RSYNC_URL $RSYNC_DEST`
exit $ret
