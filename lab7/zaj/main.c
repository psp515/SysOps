#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

int main() {
    int shm_id;
    key_t key = 1234;
    char *shared_memory;
    char buffer[100];

    // Tworzenie pamięci wspólnej
    shm_id = shmget(key, 1024, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }

    // Przylączenie pamięci wspólnej do procesu
    shared_memory = shmat(shm_id, NULL, 0);
    if (shared_memory == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    // Zapisanie wartości do pamięci wspólnej
    sprintf(buffer, "Hello World!");
    strncpy(shared_memory, buffer, 100);

    // Odcinienie pamięci wspólnej od procesu
    if (shmdt(shared_memory) == -1) {
        perror("shmdt");
        exit(1);
    }

    int x = fork();

    if (x == 0) {
        // Ponowne przylączenie pamięci wspólnej do procesu
        shared_memory = shmat(shm_id, NULL, 0);
        if (shared_memory == (char *) -1) {
            perror("shmat");
            exit(1);
        }

        // Odczytanie wartości z pamięci wspólnej i wypisanie jej na ekran
        printf("Odczytana wartość: %s\n", shared_memory);

        // Odcinienie pamięci wspólnej od procesu
        if (shmdt(shared_memory) == -1) {
            perror("shmdt");
            exit(1);
        }

        exit(0);
    }

    printf("Start Wait\n");
    wait(NULL);
    printf("End Wait\n");

    // Usunięcie pamięci wspólnej
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}