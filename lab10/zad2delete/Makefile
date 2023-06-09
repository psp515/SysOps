CC = gcc
CFLAGS = -Wall
FILE1 = ./server
FILE2 = ./client

compile_server:
	$(CC) $(CFLAGS) -c  $(FILE1).c 
	$(CC) $(CFLAGS) -o $(FILE1) $(FILE1).o -lpthread

compile_client:
	$(CC) $(CFLAGS) -c  $(FILE2).c 
	$(CC) $(CFLAGS) -o $(FILE2) $(FILE2).o

run_server:
	make compile_sender
	$(FILE1)

run_client:
	make compile_catcher
	$(FILE2)

all:
	make run_server
	make run_client
	make clean

clean:
	rm -rf _.* *.so *.gch *.o *.a $(FILE1) $(FILE2)
