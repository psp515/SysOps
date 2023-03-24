#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define MAX_LEN 10

static int requests = 0;
static int mode = 0;

void print_numbers()
{
    for (int i = 1; i <= 100; i++) 
        printf("C: %d\n", i);
    
}

void print_time()
{
    time_t t;
    time(&t);
    printf("C: Current time: %s\n", ctime(&t));
}

void off()
{
    printf("C: Finished\n");
    exit(0);
}

void handler(int sig, siginfo_t* info, void* context)
{

    printf("C: Signal: %d\n", sig);
    printf("C: From Pid: %d\n", info->si_pid);
    int new_mode = info->si_value.sival_int;
    printf("C: Mode: %d\n", new_mode);

    requests += 1;
    mode = new_mode;

    printf("C: Send Confirmation.\n");
    int resp = kill(info->si_pid, SIGUSR1);
    if(resp == -1)
    {
        perror("C: Catcher to sender error.");
        exit(1);
    }
}

int main()
{
    pid_t pid = getpid();
    printf("-------------Catcher started.-------------\n");
    printf("C: Pid: %d\n", pid);

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);

    if (sigaction(SIGUSR1, &act, NULL) < 0) {
        perror("C: Sigaction error.\n");
        exit(1);
    }

    pid_t scnd_pid = fork();

    if(scnd_pid == 0)
    {
        char str_pid[MAX_LEN];
        sprintf(str_pid,"%d", pid);
        int resp = execl("./sender", "sender", str_pid, "2", "2", "5", NULL);

        if(resp == -1)
        {
            perror("C: Sender not started.");
            exit(1);
        }
    }

    while(mode != 5)
    {
        switch(mode)
        {
            case 1:
                print_numbers();
                mode = 0;
                break;
            case 2:
                print_time();
                mode = 0;
                break;
            case 3:
                printf("C: Received data: %d\n", requests);
                mode = 0;
                break;
            case 4:
                print_time();
                sleep(1);
                break;
            default:
                continue;
        }

    }

    printf("Catcher finished.\n");
}