#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define PIPE "./squareFIFO"

int main() {
    /***********************************
    * odczytaj z potoku nazwanego PIPE zapisana tam wartosc i przypisz ja do zmiennej val
    * posprzataj
    ************************************/

    FILE * file = fopen(PIPE,"r");

    char buff[12];
    fread(buff, 1, 12, file);

    int val;
    sprintf(buff, 1, 12,"%d", val);

    fclose(file);
}
