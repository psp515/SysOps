cc = gcc -Wall -g

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
	$(cc) -fPIC -shared -o lib.so build/lib.o

main_shared: main.c shared
	$(cc) main.c -lib -L. -o main.out

main_dynamic: main.c shared
	$(cc) main.c -o main.out -ldl -D dynamic

clean:
	rm -Rf build *.out *.so *.o *.a