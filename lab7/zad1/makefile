cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

client: dir client.c
	$(cc) -c client.c -o build/client.o
	$(cc) build/client.o -o build/client

barber: dir barber.c
	$(cc) -c barber.c -o build/barber.o
	$(cc) build/barber.o -o build/barber

run: dir barber client main.c
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o build/main.out
	./build/main.out 2 2 7 6

clean:
	rm -Rf build *.o *.so *.a *.sa *.out