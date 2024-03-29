#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>
#include <semaphore.h>

#define FILE_NAME "common.txt"
#define SEM_NAME "my_kol_sem"

int main(int argc, char **args) {
    if (argc != 4) {
        printf("Not a suitable number of program parameters\n");
        return 1;
    }
    /************************************
      Utworz semafor posixowy. Ustaw jego wartosc na 1
     *************************************/

    int fd = open(FILE_NAME, O_WRONLY | O_CREAT , 0644);

    int parentLoopCounter = atoi(args[1]);
    int childLoopCounter = atoi(args[2]);

    char buf[50];
    pid_t childPid;
    int max_sleep_time = atoi(args[3]);
    if ((childPid = fork())) {
        int status = 0;
        srand((unsigned) time(0));

        while (parentLoopCounter--) {
            int s = rand() % max_sleep_time + 1;
            sleep(s);
            /******************************************
              Sekcja krytyczna. Zabezpiecz dostep semaforem
             ******************************************/

            /****************************************************
              Koniec sekcji krytycznej
             ****************************************************/
            sem_post(sem_id);
        }
        waitpid(childPid, &status, 0);
    } else {
        srand((unsigned) time(0));
        while (childLoopCounter--) {
            int s = rand() % max_sleep_time + 1;
            sleep(s);
            /******************************************
              Sekcja krytyczna. Zabezpiecz dostep semaforem
             ******************************************/

            /****************************************************
              Koniec sekcji krytycznej
             ****************************************************/
            sem_post(sem_id);
        }
        _exit(0);
    }
    /***************************************
      Posprzataj semafor
     ***************************************/

    close(fd);
    return 0;
}


