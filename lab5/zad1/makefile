cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

run: dir main.c
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o build/main.out

run_send : run
	./build/main.out psp515 psp515 psp515Tresc

run_sort_nadawca : run
	./build/main.out nadawca

run_sort_data : run
	./build/main.out data
	
clean:
	rm -Rf build *.o *.so *.a *.sa *.out