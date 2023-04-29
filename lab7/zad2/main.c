#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include "common.h"

static sem_t* sem_queue;
static sem_t* sem_chairs;
static sem_t* sem_barbers;
static sem_t* sem_mutex;

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

    int descriptor = shm_open(PROJECT, O_CREAT | O_RDWR, 0644);
    //ftruncate(descriptor, SIZE);
    char *shared = (char*) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);

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

    shm_unlink(PROJECT);

    remove_semaphores();

    printf("---- Simulation succesfully finished. [SIMULATION STOP] ----\n");
    fflush(stdout);

    return 0;
}

void create_semaphores(int chairs) {

    sem_queue = sem_open(SEM_QUEUE_FNAME, O_EXCL | O_CREAT, 0644, chairs);
    if (sem_queue == SEM_FAILED) {
        perror("---- Failed creating semaphore. [SIMULATION ERROR] ----\n");
        exit(1);
    }

    sem_chairs = sem_open(SEM_CHAIRS_FNAME, O_EXCL | O_CREAT, 0644, 0);
    if (sem_chairs == SEM_FAILED) {
        perror("---- Failed creating semaphore. [SIMULATION ERROR] ----\n");
        exit(1);
    }

    sem_barbers = sem_open(SEM_BARBERS_FNAME, O_EXCL | O_CREAT, 0644, 0);
    if (sem_barbers == SEM_FAILED) {
        perror("---- Failed creating semaphore. [SIMULATION ERROR] ----\n");
        exit(1);
    }

    sem_mutex = sem_open(SEM_BUFFER_MUTEX_FNAME, O_EXCL | O_CREAT, 0644, 1);
    if (sem_mutex == SEM_FAILED) {
        perror("---- Failed creating semaphore. [SIMULATION ERROR] ----\n");
        exit(1);
    }
}

void remove_semaphores()
{
    sem_close(sem_queue);
    sem_close(sem_chairs);
    sem_close(sem_barbers);
    sem_close(sem_mutex);
}