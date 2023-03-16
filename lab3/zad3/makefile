cc = gcc -Wall -g -std=c11
route = test
chars = abcde

dir:
	mkdir -p build

run: dir main.c
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o main.out
	./main.out $(route) $(chars) > output.txt

clean:
	rm -Rf build *.out *.so *.o *.a *.test