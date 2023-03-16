cc = gcc -Wall -g -std=c11
route = test
chars = abcd

dir:
	mkdir -p build

run: dir main.c
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o build/main.out
	./build/main.out $(route) $(chars) > output.txt

clean:
	rm -Rf build *.out *.so *.o *.a *.test