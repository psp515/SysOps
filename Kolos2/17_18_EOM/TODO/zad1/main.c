#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define MAX 20
#define KEY "./path"

int main() {
    int shm_id, i;
    int* buf;

    // Wygeneruj klucz dla kolejki na podstawie KEYi znaku 'a'

    /** ADDED BY ME FROM HERE **/


    /** TILL HERE **/

    // Utwórz segment pamięci współdzielonej shmid o prawach dostępu 600,
    // rozmiarze MAX jeśli segment już istnieje, zwróć błąd, jeśli utworzenie
    // pamięci się nie powiedzie zwróć błąd i wartość 1

    /** ADDED BY ME FROM HERE **/



    /** TILL HERE **/

    // Przyłącz segment pamięci współdzielonej do procesu do buf, obsłuż błędy i
    // zwróć 1

    /** ADDED BY ME FROM HERE **/


    /** TILL HERE **/

    for (i = 0; i < MAX; i++) {
        buf[i] = i * i;
        printf("Wartość: %d \n", buf[i]);
    }

    printf("Memory written\n");

    // odłącz i usuń segment pamięci współdzielonej

    /** ADDED BY ME FROM HERE **/


    /** TILL HERE **/

    return 0;
}
