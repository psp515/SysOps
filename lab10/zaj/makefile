cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

main: dir
	$(CC) main.c -o build/main

client: dir
	$(CC) client.c -o build/client

run_main: dir
	./build/main

run_client: dir
	./build/client

clean:
	rm -Rf build *.o *.so *.a *.sa *.out