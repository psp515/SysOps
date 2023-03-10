cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

clean:
	rm -Rf build *.out *.so *.o *.a

run_test: dir
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o build/main.out
	./build/main.out >> zad_3_output.txt
	wc --bytes * >> zad_3_output.txt
