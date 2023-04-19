#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int i, pid;

    if (argc != 2) {
        printf("Not a suitable number of program arguments");
        exit(2);
    } else {
        for (i = 0; i < atoi(argv[1]); i++) {
            //*********************************************************
            // Uzupelnij petle w taki sposob aby stworzyc dokladnie argv[1]
            // procesow potomnych, bedacych dziecmi tego samego procesu
            // macierzystego. Kazdy proces potomny powinien:
            // - "powiedziec ktorym jest dzieckiem",
            // - jaki ma pid,
            // - kto jest jego rodzicem
            //******************************************************

            /* ADDED BY ME - FROM HERE */

            pid = fork();
            if(pid == -1)
            {
                printf("Fork error");
                exit(1);
            }
            else if(pid == 0)
            {
                printf("Dziecko %d sie melduje o pid %d gdzie rodzicem jest %d.\n", i+1, getpid(), getppid());
                exit(0);
            }
            /* TILL HERE*/
        }
    }
    return 0;
}
