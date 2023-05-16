#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


void handler(int sig, siginfo_t *siginfo, void *ucontext) {
    printf("Received signal %d from %d with additional value: %d\n",
            sig, siginfo->si_pid, siginfo->si_value.sival_int);

    sigset_t sset;
    sigprocmask(SIG_BLOCK, NULL, &sset);

    printf("Mask test: %d\n", sigismember(&sset, SIGPIPE));
}

/* Funkcja 'set_handler' ma ustawic 'handler' jako funkcje
 * obslugujaca sygnal SIGUSR1. W trakcie obslugi sygnalu
 * SIGUSR1 ma byc blokowana obsluga sygnalu SIGPIPE. Zwroc
 * uwage, ze funkcja 'handler' odczutuje pewna dodatkowa
 * informacje wyslana wraz z sygnalem SIGUSR1.
 */
void set_handler(void){
    
}
 
int main(void) {
    pid_t child = fork();

    if(child) {
        set_handler();
        waitpid(child, NULL, 0);
    } else {
        pid_t parent = getppid();
        pid_t me = getpid();

        sleep(1);
        sigqueue(parent, SIGUSR1, (union sigval) me);
    }

    return 0;
}
