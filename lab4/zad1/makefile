cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

run: dir main.c main_test.c
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o main.out
	$(cc) -c main_test.c -o build/main_test.o
	$(cc) build/main_test.o -o main_test
	./main.out pending 

clean:
	rm -Rf build *.out *.so *.o *.a *.test main_test
