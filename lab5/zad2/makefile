cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

run : dir main.c
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o build/main.out
	./build/main.out 0.00000001 1 >> result.txt
	./build/main.out 0.00000001 2 >> result.txt
	./build/main.out 0.00000001 5 >> result.txt
	./build/main.out 0.00000001 10 >> result.txt
	
clean:
	rm -Rf build *.o *.so *.a *.sa *.out result.txt