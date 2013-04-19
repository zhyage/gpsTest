gpsTest:gpsTest.o queue.o sllist.o
	gcc -I../ -I. -I./utils/ gpsTest.o queue.o sllist.o ../.libs/libgps.a -o gpsTest -lm -lpthread
gpsTest.o:gpsTest.c
	gcc -I../ -I. -I./utils/ -c gpsTest.c
queue.o:utils/queue.c
	gcc -I../ -I. -I./utils/ -c utils/queue.c
sllist.o:utils/sllist.c
	gcc -I../ -I. -I./utils/ -c utils/sllist.c

clean:
	rm -rf *.o
	rm -rf gpsTest
