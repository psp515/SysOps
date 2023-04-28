//
// Created by psp515 on 28/04/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>
#include "common.h"
#include <time.h>

static sem_t* sem_queue;
static sem_t* sem_chairs;
static sem_t* sem_barbers;
static sem_t* sem_mutex;

int parse(char* string)
{
    char *endptr;
    int value = strtod(string, &endptr);
    if (errno == ERANGE) {
        printf("Value out of range for double\n");
        exit(1);
    }
    else if (*endptr != '\0') {
        printf("Invalid input: '%s'\n", endptr);
        exit(1);
    }
    return value;
}

void open_semaphores()
{
    sem_queue = sem_open(SEM_QUEUE_FNAME, 0);
    sem_chairs = sem_open(SEM_CHAIRS_FNAME, 0);
    sem_barbers = sem_open(SEM_BARBERS_FNAME,, 0);
    sem_mutex = sem_open(SEM_BUFFER_MUTEX_FNAME, 0);
}

int main(int n, char* args[]) {

    if(n!= 2)
    {
        printf("Invalid number of arguemtns.");
        fflush(stdout);
        return 1;
    }

    srand(time(NULL) + getpid());

    int descriptor = shm_open(filename, O_RDWR, 0644);
    char *shared = (char*) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);

    int len = strlen(shared);
    if(len >= parse(args[1]))
    {
        printf("[CLIENT-%d] Queue full. Leaving.\n", getpid());
        fflush(stdout);
        exit(0);
    }

    open_semaphores();

    printf("[CLIENT-%d] New client. \n", getpid());
    fflush(stdout);

    sem_wait(sem_queue);
    sem_wait(sem_mutex);

    char byte = (char) (rand() % 12);
    printf("[CLIENT-%d] New client with haircut no. %d \n", getpid(), byte);
    fflush(stdout);

    shared[len] = byte;
    shared[len + 1] = '\0';

    sem_post(sem_mutex);
    sem_post(sem_barbers);
    sem_wait(sem_chairs);

    printf("[CLIENT-%d] Client done.\n", getpid());
    fflush(stdout);

    munmap(shared, SIZE);

    return 0;
}