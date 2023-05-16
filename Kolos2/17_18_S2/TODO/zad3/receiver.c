#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/** path must be an existing one **/

#define KEY "/tmp"

int main() {
    sleep(1);
    int val = 0;

    /**********************************
    Otworz kolejke systemu V "reprezentowana" przez KEY
    **********************************/

    /** ADDED BY ME FROM HERE **/



    /** TILL HERE **/

    while (1) {
        /**********************************
        Odczytaj zapisane w kolejce wartosci i przypisz je do zmiennej val
        obowiazuja funkcje systemu V
        ************************************/

        /** ADDED BY ME FROM HERE **/


        /** TILL HERE **/

        printf("%d square is: %d\n", val, val * val);
    }

    /*******************************
    posprzataj
    ********************************/

    return 0;
}
