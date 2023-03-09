cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

run: lib_main.c
	$(cc) -c lib_main.c -o build/main.o
	$(cc) build/main.o -o main.out

clean:
	rm -Rf build *.out *.so *.o *.a