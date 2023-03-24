#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

static int requests = 0;
static int is_working = 1;
static int mode = 0;

void print_numbers()
{
    for (int i = 1; i <= 100; i++) 
        printf("%d\n", i);
    
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
    int new_mode = info->si_value.sival_int;
    printf("Mode: %d\n", new_mode);

    requests += 1;

    mode = new_mode;

    if(mode == 1)
        print_numbers();
    else if (mode == 2)
        print_time();
    else if(mode == 3)
        printf("Received data: %d", requests);
    else if(mode == 5)
        off();

    kill(info->si_pid, SIGUSR1);
}

int main()
{
    pid_t pid = getpid();
    printf("Catcher started.\n");
    printf("Pid: %d\n", pid);

    execl("./sender", "sender", pid, NULL);

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