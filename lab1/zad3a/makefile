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
	$(cc) -shared -fPIC -o libdiff.so build/lib.o

main_shared: main.c shared
	$(cc)  main.c -L. -o main.out -ldiff -Wl,-rpath=.

main_dynamic: main.c shared
	$(cc) main.c -o main.out -D dynamic

main_static_test: main_static
	./main.out < test.txt > raport_static.txt

main_shared_test: main_shared
	./main.out < test.txt > raport_shared.txt

main_dynamic_test: main_dynamic
	./main.out < test.txt > raport_dynamic.txt

clean:
	rm -Rf build *.out *.so *.o *.a