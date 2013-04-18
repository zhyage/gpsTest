gpsTest:gpsTest.o
	gcc -I../ gpsTest.o ../.libs/libgps.a -o gpsTest -lm -lpthread
gpsTest.o:gpsTest.c
	gcc -I../ -c gpsTest.c

clean:
	rm -rf *.o
	rm -rf gpsTest
