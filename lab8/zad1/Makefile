CC = gcc
CFLAGS = -Wall

all: life

run: life
	./life
	
life: grid.c life.c
	$(CC) $(CFLAGS) grid.c life.c -o life -lncursesw -lpthread
	
clean:
	rm -f life
