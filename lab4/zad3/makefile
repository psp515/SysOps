cc = gcc -Wall -g

dir:
	mkdir -p build

run: dir main.c sender.c
	$(cc) -c sender.c -o build/sender.o 
	$(cc) build/sender.o -o sender
	$(cc) -c main.c -o build/main.o 
	$(cc) build/main.o -o main
	./main
	# teraz recznie uruchamiamy ./sender i wpisujamy interesujace nas komendy

clean:
	rm -Rf build *.out *.so *.o *.a *.test sender main
