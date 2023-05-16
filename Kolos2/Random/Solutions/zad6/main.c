#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>

#define THREADS_NR 9

void *hello(void *arg) {
    //Napisz funkcję wypisującą komunikat typu Hello  (wartość_TID): (numer_TID)
    // i zwracającą status o wartości równej agrumentowi przesłanemu do funkcji

    printf("Hello %ld:%d\n", pthread_self(), *(int *) arg);
    pthread_exit((void *) *(int *) arg);
}

int main(int argc, char *argv[]) {
    pthread_t hello_threads[THREADS_NR];
    int *count = malloc(sizeof(int));
    int i;

    for (i = 0; i < THREADS_NR; i++) {
        *count = i + 1;
        int result = pthread_create(&hello_threads[i], NULL, hello, (void *) count);
        if (result != 0) {
            perror("Could not create thread.");
        }
        usleep(1000);
    }
    int *hello_results[THREADS_NR];
    // Czekaj na zakończenie THREADS_NR wątków, pobierz status zakończeńczenia, zapisz go w  hello_result
    // i wypisz na ekran: Thread (indeks wątku+1) TID: (nr_TID) returned value: (status zakonczenia watku)

    for (i = 0; i < THREADS_NR; i++) {
        void *retval;
        pthread_join(hello_threads[i], &retval);
        hello_results[i] = retval;
    }

    for (i = 0; i < THREADS_NR; i++) {
        printf("Thread\t%d TID: %ld\tretval: %d\n", i + 1, hello_threads[i], hello_results[i]);
    }


    //koniec
    free(count);
    return 0;
}
