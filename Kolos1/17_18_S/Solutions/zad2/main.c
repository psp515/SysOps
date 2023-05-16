#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 3. Obustronna komunikacja między dwoma procesami za pomocą 2 pipeów

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Not a suitable number of program parameters\n");
        exit(1);
    }

    int toChildFD[2];
    int toParentFD[2];

    pipe(toChildFD);
    pipe(toParentFD);

    int val1, val2, val3 = 0;

    pid_t pid;

    if ((pid = fork()) == 0) {
        /* CHANGES FROM HERE */

        close(toChildFD[1]);
        close(toParentFD[0]);

        // odczytaj z potoku nienazwanego wartosc przekazana przez proces
        // macierzysty i zapisz w zmiennej val2

        char buff[12];
        read(toChildFD[0], buff, sizeof(buff));

        val2 = atoi(buff);

        val2 = val2 * val2;

        // wyslij potokiem nienazwanym val2 do procesu macierzysego

        sprintf(buff, "%d", val2);

        write(toParentFD[1], buff, sizeof(buff));

        close(toChildFD[0]);
        close(toParentFD[1]);

    } else {
        val1 = atoi(argv[1]);

        close(toChildFD[0]);
        close(toParentFD[1]);

        // wyslij val1 potokiem nienazwanym do priocesu potomnego

        char buff[12];
        sprintf(buff, "%d", val1);

        write(toChildFD[1], buff, sizeof(buff));

        sleep(1);

        // odczytaj z potoku nienazwanego wartosc przekazana przez proces
        // potomny i zapisz w zmiennej val3

        read(toParentFD[0], buff, sizeof(buff));

        val3 = atoi(buff);

        close(toChildFD[1]);
        close(toParentFD[0]);

        /* TILL HERE */

        printf("%d square is: %d\n", val1, val3);
    }

    return 0;
}