cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build
	mkdir -p tmp

run : dir calc.c main.c
	$(cc) -c calc.c -o build/calc.o
	$(cc) build/calc.o -o calc
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o main
	./main 0.00001 1 >> result.txt
	./main 0.0000001 2 >> result.txt
	./main 0.0000001 5 >> result.txt
	./main 0.0000001 10 >> result.txt

clean:
	rm -Rf build *.o *.so *.a *.sa *.out result.txt tmp calc main