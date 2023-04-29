#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "common.h"

static int sem_queue;
static int sem_chairs;
static int sem_barbers;
static int sem_mutex;

void create_semaphores(int chairs);
void remove_semaphores();

int main(int n, char* args[]) {

    if(n != 5)
    {
        printf("---- Invalid number of arguemnts. [SIMULATION ERROR] ----\n");
        fflush(stdout);
        return 1;
    }

    int barbers = parse(args[1]);
    int chairs = parse(args[2]);
    int clients = parse(args[3]);
    //int queue = parse(args[4]);

    key_t key = ftok(PROJECT, 0);
    int id = shmget(key, SIZE, 0644 | IPC_CREAT);
    char * shared = shmat(id, NULL, 0);

    if(shared == NULL)
        exit(EXIT_FAILURE);

    shared[0] = '\0';

    create_semaphores(chairs);

    printf("---- Simulation starts. [SIMULATION] ----\n");
    fflush(stdout);

    for(int i = 0; i < barbers; i++)
        if (fork() == 0)
            execl("build/barber", "barber", NULL);

    printf("---- Spawned all barbers. [SIMULATION RUNNING] ----\n");
    fflush(stdout);

    for(int i = 0; i < clients; i++)
        if (fork() == 0)
            execl("build/client", "client", args[4], NULL);

    printf("---- Spawned all customers. [SIMULATION RUNNING] ----\n");
    fflush(stdout);

    printf("---- Waiting for children finish. [SIMULATION RUNNING] ----\n");
    fflush(stdout);
    while(wait(NULL) > 0);
    printf("---- All children finished. [SIMULATION RUNNING] ----\n");
    fflush(stdout);

    shmctl(id, IPC_RMID, NULL);

    remove_semaphores();
    printf("---- Simulation succesfully finished. [SIMULATION STOP] ----\n");
    fflush(stdout);

    return 0;
}

void create_semaphores(int chairs) {

    key_t key = ftok(PROJECT, SEM_QUEUE_FNAME[0]);
    sem_queue = semget(key, 1, 0664 | IPC_CREAT);
    if (sem_queue == -1) {
        perror("---- Failed creating semaphore. [SIMULATION ERROR] ----\n");
        exit(1);
    }
    semctl(sem_queue, 0, SETVAL, chairs);
    key = ftok(PROJECT, SEM_CHAIRS_FNAME[0]);
    sem_chairs = semget(key, 1, 0664 | IPC_CREAT);
    if (sem_chairs == -1) {
        perror("---- Failed creating semaphore. [SIMULATION ERROR] ----\n");
        exit(1);
    }
    semctl(sem_chairs, 0, SETVAL, 0);
    key = ftok(PROJECT, SEM_BARBERS_FNAME[0]);
    sem_barbers = semget(key, 1, 0664 | IPC_CREAT);
    if (sem_barbers == -1) {
        perror("---- Failed creating semaphore. [SIMULATION ERROR] ----\n");
        exit(1);
    }
    semctl(sem_barbers, 0, SETVAL, 0);
    key = ftok(PROJECT, SEM_BUFFER_MUTEX_FNAME[0]);
    sem_mutex = semget(key, 1, 0664 | IPC_CREAT);
    if (sem_mutex == -1) {
        perror("---- Failed creating semaphore. [SIMULATION ERROR] ----\n");
        exit(1);
    }
    semctl(sem_mutex, 0, SETVAL, 1);
}

void remove_semaphores()
{
    semctl(sem_queue, 0, IPC_RMID);
    semctl(sem_chairs, 0, IPC_RMID);
    semctl(sem_barbers, 0, IPC_RMID);
    semctl(sem_mutex, 0, IPC_RMID);
}