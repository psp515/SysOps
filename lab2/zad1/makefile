cc = gcc -Wall -g -std=c11

znak1 = a
znak2 = b

dir:
	mkdir -p build

run_lib: dir lib_main.c
	$(cc) -c lib_main.c -o build/main.o
	$(cc) build/main.o -o main.out
	./main.out $(znak1) $(znak2) lib_main.c build/lib_main_copy.test >> pomiar_zad_1.txt

run_sys: dir lib_main.c
	$(cc) -c sys_main.c -o build/main.o
	$(cc) build/main.o -o main.out
	./main.out $(znak1) $(znak2)  sys_main.c build/sys_main_copy.test >> pomiar_zad_1.txt

run: run_lib run_sys

compare: run
	tr $(znak1) $(znak2) < lib_main.c >  build/tmp_lib.test;
	diff -s  build/tmp_lib.test  build/lib_main_copy.test > diff_lib.txt;

	tr $(znak1) $(znak2) < sys_main.c > build/tmp_sys.test;
	diff -s  build/tmp_sys.test build/sys_main_copy.test  > diff_sys.txt;

clean:
	rm -Rf build *.out *.so *.o *.a *.test