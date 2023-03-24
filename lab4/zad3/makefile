cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

run: dir main.c sender.c
	$(cc) -c sender.c -o build/sender.o 
	$(cc) build/sender.o -o sender
	$(cc) -c main.c -o build/main.o 
	$(cc) build/main.o -o main
	./main

clean:
	rm -Rf build *.out *.so *.o *.a *.test
