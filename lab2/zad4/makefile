cc = gcc -Wall -g -std=c11
dir_path = test

dir:
	mkdir -p build

clean:
	rm -Rf build *.out *.so *.o *.a

run_test: dir
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o -o build/main.out
	./build/main.out $(dir_path) >> zad_4_output.txt
	find $(dir_path) | xargs wc --bytes >> zad_4_output.txt
