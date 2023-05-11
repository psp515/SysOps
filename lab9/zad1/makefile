cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

run: dir
	$(CC) main.c -o build/main -lncursesw -lpthread
	./build/main

clean:
	rm -Rf build *.o *.so *.a *.sa *.out