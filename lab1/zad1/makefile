all: static shared

dir:
	mkdir -p build

static: dir lib.c
	gcc -Wall -c lib.c -o build/lib.o
	ar crs lib.a build/lib.o

shared: dir lib.c
	gcc -Wall -g -c -fPIC lib.c -o build/lib.o
	gcc -Wall -g -shared -o libd.so build/lib.o

clean:
	rm -Rf build *.out *.so *.o *.a




