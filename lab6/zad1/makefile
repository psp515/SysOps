cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

run: dir main.c
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o build/main.out
	./build/main.out 1
	
clean:
	rm -Rf build *.o *.so *.a *.sa *.out