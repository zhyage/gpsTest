gpsTest:gpsTest.o arrQueue.o positionReport.o stopAnnounce.o dllist.o transferData.o utils.o
	mipsel-linux-gcc -I../ -I. -I./utils/ gpsTest.o arrQueue.o positionReport.o stopAnnounce.o dllist.o transferData.o utils.o -o gpsTest -lm -lpthread -L../.libs -lgps
gpsTest.o:gpsTest.c
	mipsel-linux-gcc -I../ -I. -I./utils/ -c gpsTest.c
arrQueue.o:utils/arrQueue.c
	mipsel-linux-gcc -I../ -I. -I./utils/ -c utils/arrQueue.c
dllist.o:utils/dllist.c
	mipsel-linux-gcc -I../ -I. -I./utils/ -c utils/dllist.c
utils.o:utils/utils.c
	mipsel-linux-gcc -I../ -I. -I./utils/ -c utils/utils.c
positionReport.o:positionReport.c
	mipsel-linux-gcc -I../ -I. -I./utils/ -c positionReport.c
stopAnnounce.o:stopAnnounce.c
	mipsel-linux-gcc -I../ -I. -I./utils/ -c stopAnnounce.c
transferData.o:transferData.c
	mipsel-linux-gcc -I../ -I. -I./utils/ -c transferData.c

clean:
	rm -rf *.o
	rm -rf gpsTest
