cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

run_server: dir server.c
	$(cc) -c server.c -o build/server.o
	$(cc) build/server.o -o build/server.out
	./build/server.out

run_client: dir client.c
	$(cc) -c client.c -o build/client.o
	$(cc) build/client.o -o build/client.out
	./build/client.out

run_client2: dir client.c
	$(cc) -c client.c -o build/client.o
	$(cc) build/client.o -o build/client.out
	./build/client.out

run_client3: dir client.c
	$(cc) -c client.c -o build/client.o
	$(cc) build/client.o -o build/client.out
	./build/client.out

clean:
	rm -Rf build *.o *.so *.a *.sa *.out