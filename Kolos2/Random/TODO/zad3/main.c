#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/wait.h>

#define FILE_NAME "common.txt"
#define SEM_NAME "/kol_sem"
int semid;

void releaseSemaphore() {
    struct sembuf s;
    s.sem_flg = 0;
    s.sem_num = 0;
    s.sem_op = 1;
    semop(semid, &s, 1);
}

void takeSemaphore() {
    struct sembuf s;
    s.sem_flg = 0;
    s.sem_num = 0;
    s.sem_op = -1;
    semop(semid, &s, 1);
}

int main(int argc, char **args) {

    if (argc != 4) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }

    /**************************************************
    Stworz semafor systemu V
    Ustaw jego wartosc na 1
    ***************************************************/
     int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);

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

            /*****************************************
            sekcja krytyczna zabezpiecz dostep semaforem
            **********************************************/

            /*********************************
            Koniec sekcji krytycznej
            **********************************/

        }
        waitpid(childPid, &status, 0);
    } else {
        srand((unsigned) time(0));
        while (childLoopCounter--) {

            int s = rand() % max_sleep_time + 1;
            sleep(s);

            /*****************************************
            sekcja krytyczna zabezpiecz dostep semaforem
            **********************************************/



            /*********************************
            Koniec sekcji krytycznej
            **********************************/

        }
        _exit(0);
    }

    /*****************************
    posprzataj semafor
    ******************************/


    return 0;
}
     
        
