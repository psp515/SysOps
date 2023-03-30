#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>


static int usr_count = 0;
static int int_count = 0;
static int quit_count = 0;

void handler(int sig, siginfo_t* info, void* context)
{
    printf("\n");

    if (sig == SIGQUIT)
    {
        printf("Quit handled.\n");
        quit_count += 1;
    }
    else if (sig == SIGINT)
    {
        printf("Int handled.\n");
        int_count += 1;
    }
    else if (sig == SIGUSR1)
    {
        printf("User handled.\n");
        usr_count += 1;
    }
    else if (sig == SIGCHLD)
    {
        printf("Child process ended.\n");
    }
    else if (sig == SIGSTOP)
    {
        printf("Child process stopped.\n");
    }

    printf("signal: %d\n", sig);
    printf("Pid: %d\n", info->si_pid);
}

int main() 
{
    // SA_SIGINFO | SA_NOCLDSTOP -> nie wykrywa stopa ale wykrywa koniec pracy wątku
    // SA_SIGINFO -> zmienia 2 argumentowa funkcje na 3 argumentową funkcje
    // SA_SIGINFO | SA_NOCLDWAIT -> nie wykrywa zakonczenia pracy childa ale wykrywa stopa 

    struct sigaction act;
    act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);

    if (sigaction(SIGINT, &act, NULL) < 0 || 
    sigaction(SIGQUIT, &act, NULL) < 0 || 
    sigaction(SIGUSR1, &act, NULL) < 0 ||
    sigaction(SIGCHLD, &act, NULL)) {
        perror("sigaction");
        exit(1);
    }


    printf("Program działa dla 2 sygnałów SIGINT I SIGQUIT oraz sam wytwarza SIGUSR1.\n");
    printf("Program kończy działania gdy kady sygnał wystąpi 4 razy.\n");

    while(usr_count <= 3 || int_count <= 3 || quit_count <= 3) 
    {
        raise(SIGUSR1);
        printf("SIG_QUIT: %d, SIG_INT: %d, SIG_USR %d\n",quit_count, int_count, usr_count);
        sleep(3);
    }

    int pid = fork();
    
    if(pid==0)
    {
        raise(SIGUSR1);
        sleep(1);
        printf("Child process has stopped\n");
        exit(0);
    }

    printf("Waiting for child process to exit (5s)\n");
    sleep(5);
    printf("Finished\n");

    printf("SIG_QUIT: %d, SIG_INT: %d, SIG_USR %d\n",quit_count, int_count, usr_count);

} 
