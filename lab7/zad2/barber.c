//
// Created by psp515 on 28/04/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include <time.h>
#include "semaphore.h"
#include <sys/mman.h>
#include <fcntl.h>


#define TIMEOUT 10

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

int main() {
    srand(time(NULL) + getpid());

    int descriptor = shm_open(PROJECT, O_RDWR, 0644);
    char *shared = (char*) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);

    open_semaphores();

    printf("\tBarber starts working. [BARBER-%d] \n", getpid());
    fflush(stdout);

    while (1)
    {
        sem_wait(sem_barbers);
        sem_post(sem_mutex);

        if(strlen(shared) > 0) {
            char haircut = shared[0];
            memcpy(shared, shared + 1, strlen(shared) + 1);

            sem_post(sem_mutex);

            printf("\tProcessing haircut no. %d. [BARBER-%d RUNNING]\n", haircut, getpid());
            fflush(stdout);

            sleep(haircut);

            printf("\tHaircut finished haircut no. %d. [BARBER-%d RUNNING]\n", haircut, getpid());
            fflush(stdout);

            sem_post(sem_chairs);
            sem_post(sem_queue);

        }
        else
        {
            sleep(TIMEOUT);
            if (strlen(shared) == 0)
                break;
        }
    }

    munmap(shared, SIZE);

    printf("\tBarber finished work, goes to home. [BARBER-%d STOP] \n", getpid());
    fflush(stdout);

    return 0;
}