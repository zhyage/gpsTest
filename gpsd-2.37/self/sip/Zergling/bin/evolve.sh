#! /bin/sh

ROOT_DIR=/mnt/upgflash/LocalServer
ZERGLING_DIR=$ROOT_DIR/Zergling

clean()
{
	rm -rf $ZERGLING_DIR/log/* >/dev/null 2>&1
}

evolve()
{
	WGET=$ZERGLING_DIR/bin/wget
	## Check wget exists or not
	if [ ! -f $WGET ]
	then
		return -1
	fi

	## Start to down the bundle
	## wget [选项]... [URL]...
	## 选项:
	## -o,  --output-file=FILE        将日志信息写入 FILE。
	## -t,  --tries=NUMBER            设置重试次数为 NUMBER (0 代表无限制)。
	## -O,  --output-document=FILE    将文档写入 FILE。
	## -c,  --continue                断点续传下载文件。
	## -w,  --wait=SECONDS            等待间隔为 SECONDS 秒。
	rm -f $ROOT_DIR/Zergling.tar.bz2
	$WGET -t 5 -o $ZERGLING_DIR/log/progress.log -O $ROOT_DIR/Zergling.tar.bz2 -c -w 15 http://10.251.0.54:2244/app/Zergling.tar.bz2
	if [ $? != 0 ]
	then
		return -1
	fi

	## Kill the zergling first
	killall zergling

	clean

	## Extract the Zergling.tar.bz2
	tar xjvf $ROOT_DIR/Zergling.tar.bz2 -C $ROOT_DIR >/dev/null 2>&1
	if [ $? != 0 ]
	then
		return -1
	fi

	## Grant the binaries executable
	chmod +x $ZERGLING_DIR/bin/*

	## Bring zergling up
	$ZERGLING_DIR/bin/zergling &

	rm -f $ROOT_DIR/Zergling.tar.bz2

	return 0
}

evolve 
