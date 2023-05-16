#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/** path must be an existing one **/

#define KEY "/tmp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }

    /******************************************************
    Utworz kolejke komunikatow systemu V "reprezentowana" przez KEY
    Wyslij do niej wartosc przekazana jako parametr wywolania programu
    Obowiazuja funkcje systemu V
    ******************************************************/

    /** ADDED BY ME FROM HERE **/



    /** TILL HERE **/

    return 0;
}
