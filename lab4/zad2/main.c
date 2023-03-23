#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>


static int chld_count = 0;
static int int_count = 0;
static int quit_count = 0;

void handler(int sig, siginfo_t* info, void* context)
{
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
    else if (sig == SIGCHLD)
    {
        printf("Chld handled.\n");
        chld_count += 1;
    }

    printf("signal: %d\n", sig);
    printf("Pid: %d\n", info->si_pid);
}

int main() 
{
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    act.sa_handler = handler;

    int resp = sigaction(SIGINT, &act, NULL);
    if (resp < 0) {
        perror("sigaction");
        exit(1);
    }
    resp = sigaction(SIGQUIT, &act, NULL);
    if (resp < 0) {
        perror("sigaction");
        exit(1);
    }
    resp = sigaction(SIGCHLD, &act, NULL);
    if (resp < 0) {
        perror("sigaction");
        exit(1);
    }

    while(chld_count <= 3 || int_count <= 3 || quit_count <= 3) 
    {
        int pid = fork();

        if(pid==-1)
        {
            perror("Fork problems");
        }
        else if(pid == 0)
        {
            printf("%d\n", chld_count);
            printf("New process started\n");
            raise(SIGCHLD);
            exit(0);
        }

        printf("SIG_QUIT: %d, SIG_INT: %d, SIG_CHLD %d\n",quit_count, int_count, chld_count);

        sleep(1000);
    }

    printf("SIG_QUIT: %d, SIG_INT: %d, SIG_CHLD %d\n",quit_count, int_count, chld_count);

} 
