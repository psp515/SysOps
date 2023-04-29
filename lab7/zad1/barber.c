//
// Created by psp515 on 28/04/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "common.h"
#include <time.h>
#define TIMEOUT 10

static int sem_queue;
static int sem_chairs;
static int sem_barbers;
static int sem_mutex;

void acquire(int sem) {
    struct sembuf operation = { 0, -1, 0 };
    if(semop(sem, &operation, 1) == -1) {
        perror("aquire");
    }
}

void release(int sem) {
    struct sembuf operation = { 0, 1, 0 };
    if(semop(sem, &operation, 1) == -1){
        perror("aquire");
    }
}

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

int main() {
    srand(time(NULL) + getpid());

    key_t key = ftok(PROJECT, 0);
    int id = shmget(key, SIZE, 0644 | IPC_CREAT);
    char * shared = shmat(id, NULL, 0);

    open_semaphores();

    printf("\tBarber starts working. [BARBER-%d] \n", getpid());
    fflush(stdout);

    while (1)
    {
        acquire(sem_barbers);
        release(sem_mutex);

        if(strlen(shared) > 0) {
            char haircut = shared[0];
            memcpy(shared, shared + 1, strlen(shared) + 1);

            release(sem_mutex);

            printf("\tProcessing haircut no. %d. [BARBER-%d RUNNING]\n", haircut, getpid());
            fflush(stdout);

            sleep(haircut);

            printf("\tHaircut finished haircut no. %d. [BARBER-%d RUNNING]\n", haircut, getpid());
            fflush(stdout);

            release(sem_chairs);
            release(sem_queue);

        }
        else
        {
            sleep(TIMEOUT);
            if (strlen(shared) == 0)
                break;
        }
    }

    shmdt(&id);

    printf("\tBarber finished work, goes to home. [BARBER-%d STOP] \n", getpid());
    fflush(stdout);

    return 0;
}