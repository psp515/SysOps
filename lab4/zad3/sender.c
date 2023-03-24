#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void send_signal(pid_t pid, int signal_num, int value) {
    union sigval sigval_value;
    sigval_value.sival_int = value;
    sigqueue(pid, signal_num, sigval_value);
}

void wait_for_signal(int signal_num) {
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, signal_num);
    while(1) {
        sigsuspend(&sigset);
        return;
    }
}

int main(int n, char *args[]) {
    if(n < 2) {
        printf("Usage: ./sender <catcher_pid> <mode1> <mode2> ...\n");
        exit(1);
    }

    pid_t catcher_pid = atoi(args[1]);
    int time_span = min(2, atoi(args[2]));

    int modes[n-2];

    for(int i = 2; i < n; i++) {
        modes[i-2] = atoi(args[i]);

        if(modes[i-2] < 1 || modes[i-2]> 5)
        {
            perror("Data Error. Ending program.");
            send_signal(catcher_pid, SIGUSR1, 5);
            wait_for_signal(SIGUSR1);
            exit(1);
        }
    }

    for(int i = 0; i < n-2; i++) {
        int mode = modes[i];
        printf("Sending mode %d\n", mode);
        send_signal(catcher_pid, SIGUSR1, mode);
        wait_for_signal(SIGUSR1);
    }

    printf("All modes sent successfully!\n");

    return 0;
}