cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

compile_server: dir server.c
	$(cc) -c server.c -o build/server.o -lpthread
	$(cc) build/server.o -o build/server -lpthread

compile_client: dir client.c
	$(cc) -c client.c -o build/client.o -lpthread
	$(cc) build/client.o -o build/client -lpthread

server: compile_server
	./build/server 12345 \tmp\sock1234554

client: compile_client
	./build/client tmp 12345 127.0.0.1

client2: compile_client
	./build/client tmp2 12345 127.0.0.1

clean:
	rm -rf _.* *.so *.gch *.o *.a build *.txt
