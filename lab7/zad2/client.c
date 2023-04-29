//
// Created by psp515 on 28/04/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include <time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "semaphore.h"

static sem_t* sem_queue;
static sem_t* sem_chairs;
static sem_t* sem_barbers;
static sem_t* sem_mutex;

void open_semaphores()
{
    sem_queue = sem_open(SEM_QUEUE_FNAME, 0);
    sem_chairs = sem_open(SEM_CHAIRS_FNAME, 0);
    sem_barbers = sem_open(SEM_BARBERS_FNAME, 0);
    sem_mutex = sem_open(SEM_BUFFER_MUTEX_FNAME, 0);
}

int main(int n, char* args[]) {

    if(n!= 2)
    {
        printf("\t\tInvalid number of arguments. [CLIENT-%d ERROR]", getpid());
        fflush(stdout);
        return 1;
    }

    printf("\t\tNew client. [CLIENT-%d] \n", getpid());
    fflush(stdout);

    srand(time(NULL));

    int descriptor = shm_open(PROJECT, O_RDWR, 0644);
    char *shared = (char*) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);

    int len = strlen(shared);
    if(len >= parse(args[1]))
    {
        printf("\t\tQueue full. Leaving. [CLIENT-%d STOPS] \n", getpid());
        fflush(stdout);
        exit(0);
    }

    open_semaphores();

    sem_wait(sem_queue);
    sem_wait(sem_mutex);

    char byte = (char) (rand() % 4);
    printf("\t\tNew client with haircut no. %d [CLIENT-%d RUNNING]\n", byte, getpid());
    fflush(stdout);

    shared[len] = byte;
    shared[len + 1] = '\0';

    sem_post(sem_mutex);
    sem_post(sem_barbers);
    sem_wait(sem_chairs);

    printf("\t\tClient done. [CLIENT-%d STOPS]\n", getpid());
    fflush(stdout);

    munmap(shared, SIZE);

    return 0;
}