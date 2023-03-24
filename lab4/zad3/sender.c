#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void send_signal(pid_t pid, int signal_num, int value) {
    union sigval sigval_value;
    sigval_value.sival_int = value;
    sigqueue(pid, signal_num, sigval_value);
}

void handler(int sig, siginfo_t* info, void* context)
{
    printf("S: Confirmation received.\n");
}

int main(int n, char **args) {
     if(n < 2) {
        printf("Usage: ./sender <catcher_pid>\n");
        exit(1);
    }

    pid_t catcher_pid = atoi(args[1]);

    printf("------------- Sender started.--------------\n");
    printf("S: Pid: %d\n", getpid());
    printf("S: Catcher pid: %d\n", catcher_pid);
    printf("S: Enter numbers in the range 1-5:\n");

    int modes[n-2];
    for(int i = 2; i < n; i++) {
        modes[i-2] = atoi(args[i]);

        if(modes[i-2] < 1 || modes[i-2]> 5)
        {
            perror("Data Error. Ending program.\n");
            send_signal(catcher_pid, SIGUSR1, 5);
            exit(1);
        }
    }

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_handler = handler;
    if (sigaction(SIGUSR1, &act, NULL) < 0) {
        perror("S: Sigaction error.\n");
        exit(1);
    }

    for(int i = 0; i < n-2; i++) {
        int mode = modes[i];
        printf("S: Sending mode %d\n", mode);
        send_signal(catcher_pid, SIGUSR1, mode);
        printf("S: Signal send %d\n", mode);
        sigsuspend(&act.sa_mask);
        printf("S: Signal received %d\n", mode);
    }


    printf("S: All modes sent successfully!\n");

    return 0;
}

