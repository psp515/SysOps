#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_NAME "/kol_shm"
#define MAX_SIZE 1024

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }

    /***************************************
    Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez
    SHM_NAME zapisz tam wartosc przekazana jako parametr wywolania programu
    posprzataj
    *****************************************/

    /** ADDED BY ME FROM HERE **/


    /** not told to clean up ?? */

    /** TILL HERE **/

    return 0;
}
