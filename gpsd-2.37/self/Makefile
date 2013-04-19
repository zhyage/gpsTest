gpsTest:gpsTest.o queue.o positionReport.o
	gcc -I../ -I. -I./utils/ gpsTest.o queue.o positionReport.o ../.libs/libgps.a -o gpsTest -lm -lpthread
gpsTest.o:gpsTest.c
	gcc -I../ -I. -I./utils/ -c gpsTest.c
queue.o:utils/queue.c
	gcc -I../ -I. -I./utils/ -c utils/queue.c
positionReport.o:positionReport.c
	gcc -I../ -I. -I./utils/ -c positionReport.c

clean:
	rm -rf *.o
	rm -rf gpsTest
