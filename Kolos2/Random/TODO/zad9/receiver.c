#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define KEY  "/home/ajris"

struct mess {
    long mtype;
    long value;
};

int main() {
    sleep(1);
    /**********************************
    Otworz kolejke systemu V "reprezentowana" przez KEY
    **********************************/

    /**********************************
    Odczytaj zapisane w kolejce wartosci i przypisz je do zmiennej val
    obowiazuja funkcje systemu V
    ************************************/
    struct mess m;
    int val;

    printf("%d square is: %d\n", val, val * val);
    /*******************************
    posprzataj
    ********************************/

    return 0;
}
