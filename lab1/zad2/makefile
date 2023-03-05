cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

static: dir lib.c
	gcc -Wall -c lib.c -o build/lib.o
	ar crs build/lib.a build/lib.o

main_static: main.c static
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o build/lib.a -o main.out

shared: dir lib.c
	$(cc) -c -fPIC lib.c -o build/lib.o
	$(cc) -shared -fPIC -o liblib.so build/lib.o

main_shared: main.c shared
	gcc -Wall -g main.c -L. -o main.out -llib -Wl,-rpath=.

main_dynamic: main.c shared
	$(cc) main.c -o main.out -ldl -D dynamic

clean:
	rm -Rf build *.out *.so *.o *.a