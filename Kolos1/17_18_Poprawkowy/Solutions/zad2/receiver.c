#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PIPE "./squareFIFO"

int main() {
    int val = 0;
    /***********************************
     * odczytaj z potoku nazwanego PIPE zapisana tam wartosc i przypisz ja do
     *zmiennej val posprzataj
     ************************************/

    /* FROM HERE */

    int file = open(PIPE, O_RDONLY);

    char buff[12];
    read(file, buff,12);

    sscanf(buff, "%d", &val);

    close(file);

    /* TILL HERE */

    printf("%d square is: %d\n", val, val * val);
    return 0;
}
