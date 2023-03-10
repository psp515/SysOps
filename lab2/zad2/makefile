cc = gcc -Wall -g -std=c11

dir:
	mkdir -p build

clean:
	rm -Rf build *.out *.so *.o *.a

run_test: dir
	$(cc) -c main_block.c -o build/main_block.o
	$(cc) build/main_block.o -o main_block.out
	./main_block.out test.txt build/output1r.txt 1 >> pomiar_zad_2.txt
	./main_block.out build/output1r.txt build/output1.txt 1 >> pomiar_zad_2.txt
	diff -s test.txt build/output1.txt >> diff1.txt
	./main_block.out test.txt build/output1024r.txt 1024 >> pomiar_zad_2.txt
	./main_block.out build/output1024r.txt build/output1024.txt 1024 >> pomiar_zad_2.txt
	diff -s test.txt build/output1024.txt >> diff1024.txt
