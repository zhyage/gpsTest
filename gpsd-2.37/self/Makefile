gpsTest:gpsTest.o arrQueue.o positionReport.o stopAnnounce.o dllist.o
	gcc -I../ -I. -I./utils/ gpsTest.o arrQueue.o positionReport.o stopAnnounce.o dllist.o ../.libs/libgps.a -o gpsTest -lm -lpthread
gpsTest.o:gpsTest.c
	gcc -I../ -I. -I./utils/ -c gpsTest.c
arrQueue.o:utils/arrQueue.c
	gcc -I../ -I. -I./utils/ -c utils/arrQueue.c
dllist.o:utils/dllist.c
	gcc -I../ -I. -I./utils/ -c utils/dllist.c
positionReport.o:positionReport.c
	gcc -I../ -I. -I./utils/ -c positionReport.c
stopAnnounce.o:stopAnnounce.c
	gcc -I../ -I. -I./utils/ -c stopAnnounce.c

clean:
	rm -rf *.o
	rm -rf gpsTest
