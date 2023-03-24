#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

static int requests = 0;
static int is_working = 1;
static int mode = 0;

void print_numbers()
{
    for (int i = 1; i <= 100; i++) {
        printf("%d\n", i);
    }
}

void print_time()
{
    time_t t;
    time(&t);
    printf("Current time: %s\n", ctime(&t));
}

void off()
{
    printf("Finished\n");
    exit(0);
}

void handler(int sig, siginfo_t* info, void* context)
{
    if(sig != SIGUSR1)
        return;

    printf("Signal: %d\n", sig);
    printf("From Pid: %d\n", info->si_pid);
    int mode = info->si_value.sival_int;
    printf("Mode: %d\n", mode);

    kill(info->si_pid, SIGUSR1);
    
    // sender

    //

}

int main()
{
    printf("Catcher started.\n");
    printf("Pid: %d\n", getpid());

    //execl("","", NULL);

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);

    if (sigaction(SIGUSR1, &act, NULL) < 0) {
        perror("Sigaction error.\n");
        exit(1);
    }


    while(1)
    {
        if(mode == 4)
            print_time();
        
        sleep(1);
    }
}