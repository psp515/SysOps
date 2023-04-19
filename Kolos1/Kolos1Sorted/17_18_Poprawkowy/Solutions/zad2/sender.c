#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PIPE "./squareFIFO"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }

    // utworz potok nazwany pod sciezka reprezentowana przez PIPE
    // zakladajac ze parametrem wywolania programu jest liczba calkowita
    // zapisz te wartosc jako int do potoku i posprzataj

    /* FROM HERE */

    mkfifo(PIPE, 0777);

    int file = open(PIPE, O_WRONLY);

    char buff[12];

    sscanf(buff, "%s", argv[1]);

    write(file,buff, sizeof(buff));

    close(file);

    /* TILL HERE */

    return 0;
}
