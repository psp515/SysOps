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

static int sem_queue;
static int sem_chairs;
static int sem_barbers;
static int sem_mutex;

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

int main(int n, char* args[]) {

    if(n!= 2)
    {
        printf("Invalid number of arguemtns.");
        fflush(stdout);
        return 1;
    }

    printf("[CLIENT-%d] New client. \n", getpid());
    fflush(stdout);

    srand(time(NULL) + getpid());

    key_t key = ftok(PROJECT_IDENTIFIER, 0);
    int id = shmget(key, SIZE, 0644 | IPC_CREAT);
    char * shared = shmat(id, NULL, 0);
    int len = strlen(shared);
    if(len >= parse(args[1]))
    {
        printf("[CLIENT-%d] Queue full. Leaving.\n", getpid());
        fflush(stdout);
        exit(0);
    }

    open_semaphores();

    aquire(sem_queue);
    aquire(sem_mutex);

    char byte = (char) (rand() % 12);
    printf("[CLIENT-%d] New client with haircut no. %d \n", getpid(), byte);
    fflush(stdout);

    shared[len] = byte;
    shared[len + 1] = '\0';

    release(sem_mutex);
    release(sem_barbers);
    aquire(sem_chairs);

    printf("[CLIENT-%d] Client done.\n", getpid());
    fflush(stdout);

    shmdt(id);

    return 0;
}