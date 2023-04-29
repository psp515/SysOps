//
// Created by psp515 on 28/04/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/sem.h>
#include "common.h"
#include <time.h>

static int sem_queue;
static int sem_chairs;
static int sem_barbers;
static int sem_mutex;

void open_semaphores() {

    key_t key = ftok(PROJECT, SEM_QUEUE_FNAME[0]);
    sem_queue = semget(key, 1, 0);
    key = ftok(PROJECT, SEM_CHAIRS_FNAME[0]);
    sem_chairs = semget(key, 1, 0);
    key = ftok(PROJECT, SEM_BARBERS_FNAME[0]);
    sem_barbers = semget(key, 1, 0);
    key = ftok(PROJECT, SEM_BUFFER_MUTEX_FNAME[0]);
    sem_mutex = semget(key, 1, 0);
}

void acquire(int sem) {
    struct sembuf operation = { 0, -1, 0 };
    if(semop(sem, &operation, 1) == -1)
    {
        printf("\t\tAcquire resource error. [CLIENT-%d ERROR]", getpid());
        fflush(stdout);
        exit(1);
    }
}

void release(int sem) {
    struct sembuf operation = { 0, 1, 0 };
    if(semop(sem, &operation, 1) == -1)
    {
        printf("\t\tRelease resource error. [CLIENT-%d ERROR]", getpid());
        fflush(stdout);
        exit(1);
    }
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

    key_t key = ftok(PROJECT, 0);
    int id = shmget(key, SIZE, 0644 | IPC_CREAT);
    char * shared = shmat(id, NULL, 0);
    int len = strlen(shared);
    if(len >= parse(args[1]))
    {
        printf("\t\tQueue full. Leaving. [CLIENT-%d STOPS] \n", getpid());
        fflush(stdout);
        exit(0);
    }

    open_semaphores();

    acquire(sem_queue);
    acquire(sem_mutex);

    char byte = (char) (rand() % 3);
    printf("\t\tNew client with haircut no. %d [CLIENT-%d RUNNING]\n", byte, getpid());
    fflush(stdout);

    shared[len] = byte;
    shared[len + 1] = '\0';

    release(sem_mutex);
    release(sem_barbers);
    acquire(sem_chairs);

    printf("\t\tClient done. [CLIENT-%d STOPS]\n", getpid());
    fflush(stdout);

    shmdt(&id);

    return 0;
}