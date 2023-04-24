cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

run_server: dir main.c
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o build/main.out
	./build/main.out

run_client: dir client.c
	$(cc) -c client.c -o build/client.o
	$(cc) build/client.o -o build/client.out
	./build/client.out

run_client2: dir client.c
	$(cc) -c client.c -o build/client.o
	$(cc) build/client.o -o build/client.out
	./build/client.out
	
clean:
	rm -Rf build *.o *.so *.a *.sa *.out