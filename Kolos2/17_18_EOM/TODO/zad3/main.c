#define _BSD_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define THREADS_NR 9

void* hello(void* arg) {
    // Napisz funkcję wypisującą komunikat typu Hello  (wartość_TID):
    // (numer_TID)
    // i zwracającą status o wartości równej agrumentowi przesłanemu do funkcji

    /** ADDED BY ME FROM HERE **/


    /** TILL HERE **/

    // koniec

    pthread_exit(*(int*)arg);
}

int main(int argc, char* argv[]) {
    pthread_t hello_threads[THREADS_NR];
    int* count = malloc(sizeof(int));
    int i;

    for (i = 0; i < THREADS_NR; i++) {
        *count = i + 1;

        int result =
            pthread_create(&hello_threads[i], NULL, hello, (void*)count);
        if (result != 0) {
            perror("Could not create thread.");
        }

        usleep(1000);
    }

    int* hello_results[THREADS_NR];

    // Czekaj na zakończenie THREADS_NR wątków, pobierz status zakończeńczenia,
    // zapisz go w  hello_result i wypisz na ekran: Thread (indeks wątku+1) TID:
    // (nr_TID) returned value: (status zakonczenia watku)

    /** ADDED BY ME FROM HERE **/


    /** TILL HERE **/

    // koniec
    free(count);
    return 0;
}
