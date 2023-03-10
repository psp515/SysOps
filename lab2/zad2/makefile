cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

clean:
	rm -Rf build *.out *.so *.o *.a

run_test: dir
	$(cc) -c main_block.c -o build/main_block.o
	$(cc) build/main_block.o -o main_block.out
	./main_block.out test.txt build/output1.txt 1 >> pomiar_zad_2.txt