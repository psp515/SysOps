#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>

#include "common.h"

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

static sem_t* sem_queue;
static sem_t* sem_chairs;
static sem_t* sem_barbers;
static sem_t* sem_mutex;

void create_semaphores(int chairs);
void remove_semaphores();

int main(int n, char* args[]) {

    if(n != 5)
    {
        printf("Invalid number of arguemnts\n");
        return 1;
    }

    int barbers = parse(args[1]);
    int chairs = parse(args[2]);
    int clients = parse(args[3]);
    //int queue = parse(args[4]);

    int descriptor = shm_open(PROJECT_IDENTIFIER, O_CREAT | O_RDWR, 0644);
    ftruncate(descriptor, SIZE);
    char *shared = (char*) mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);

    shared[0] = '\0';

    create_semaphores(chairs);

    for(int i = 0; i < barbers; i++)
        if (fork() == 0)
            execl("barber", "barber", NULL);

    printf("Spawned all barbers.\n");
    fflush(stdout);
    for(int i = 0; i < clients; i++)
        if (fork() == 0)
            execl("client", "client", args[4], NULL);

    printf("Spawned all customers.\n");
    fflush(stdout);

    while(wait(NULL) > 0);

    shm_unlink(PROJECT_IDENTIFIER);

    remove_semaphores();

    return 0;
}

void create_semaphores(int chairs) {

    sem_open(sem_queue, O_CREAT | O_EXCL, 0644, chairs)
    if (sem_queue == SEM_FAILED) {
        perror("Creating a semaphore failed.");
        exit(1);
    }

    sem_open(sem_chairs, O_CREAT | O_EXCL, 0644, 0)
    if (sem_chairs == SEM_FAILED) {
        perror("Creating a semaphore failed.");
        exit(1);
    }

    sem_open(sem_barbers, O_CREAT | O_EXCL, 0644, 0)
    if (sem_barbers == SEM_FAILED) {
        perror("Creating a semaphore failed.");
        exit(1);
    }

    sem_open(sem_mutex, O_CREAT | O_EXCL, 0644, 1)
    if (sem_mutex == SEM_FAILED) {
        perror("Creating a semaphore failed.");
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