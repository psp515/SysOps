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
#define TIMEOUT 1000000

static int sem_queue;
static int sem_chairs;
static int sem_barbers;
static int sem_mutex;

void aquire(int sem) {
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

    key_t key = ftok(PROJECT_IDENTIFIER, SEM_QUEUE_FNAME[0]);
    sem_queue = semget(key, 1, 0);
    key = ftok(PROJECT_IDENTIFIER, SEM_CHAIRS_FNAME[0]);
    sem_chairs = semget(key, 1, 0);
    key = ftok(PROJECT_IDENTIFIER, SEM_BARBERS_FNAME[0]);
    sem_barbers = semget(key, 1, 0);
    key = ftok(PROJECT_IDENTIFIER, SEM_BUFFER_MUTEX_FNAME[0]);
    sem_mutex = semget(key, 1, 0);
}

int main() {
    srand(time(NULL) + getpid());

    key_t key = ftok(PROJECT_IDENTIFIER, 0);
    int id = shmget(key, SIZE, 0644 | IPC_CREAT);
    char * shared = shmat(id, NULL, 0);

    open_semaphores();

    printf("\t[BARBER-%d] Spawned\n", getpid());
    fflush(stdout);

    while (1)
    {
        aquire(sem_barbers);
        release(sem_mutex);

        if(strlen(shared) > 0) {
            char haircut = shared[0];
            memcpy(shared, shared + 1, strlen(shared) + 1);

            release(sem_mutex);

            printf("\t[BARBER-%d] Processing hairuct no. %d\n", getpid(), haircut);
            fflush(stdout);

            int sleep = haircut * 1000;
            usleep(sleep);

            printf("\t[BARBER-%d] Done with hairuct no. %d\n", getpid(), haircut);
            fflush(stdout);

            release(sem_chairs);
            release(sem_queue);

        }
        else
        {
            usleep(TIMEOUT);
            if (strlen(shared) == 0)
                break;
        }
    }

    shmdt(id);

    printf("\t[BARBER-%d] Finished work.\n", getpid());

    return 0;
}