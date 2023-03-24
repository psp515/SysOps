#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

void handler(int signal)
{   
    printf("Received %d\n", signal);
}

void is_pending(){

    sigset_t pending;
    sigpending(&pending);
    if (sigismember(&pending, SIGUSR1)) 
        printf("Pending.\n");
    else 
        printf("Not pending.\n");
        
}

int main(int n, char** args)
{
    if (n!=2)
    {
        printf("Invalid number of arguments. Should be 2.");
        exit(1);
    }

    if(strcmp(args[1], "ignore") == 0)
    {
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(args[1], "handler") == 0)
    {
        signal(SIGUSR1, handler); 
    }
    else if(strcmp(args[1], "mask") == 0 || strcmp(args[1], "pending") == 0)
    {
        sigset_t blocked;
        sigemptyset(&blocked);
        sigaddset(&blocked, SIGUSR1);
        sigprocmask(SIG_BLOCK, &blocked, NULL);

    }
    else
    {
        printf("Invalid argument. Should be ignore or handler or mask or pending.");
        exit(1);
    }
    
    printf("Raise in parent.\n");
    raise(SIGUSR1);
    is_pending();

    pid_t pid = fork();

    if(pid == -1)
    {
        perror("Failed to fork\n");
        exit(1);
    }
    else if (pid == 0)
    {
        printf("Child\n");
        if(strcmp(args[1], "pending") != 0)
        {
            printf("Raise in child.\n");
            raise(SIGUSR1);
        }
        
        is_pending();

        if(strcmp(args[1], "handler") != 0)
        {
            int x = execl("./main_test", "main_test", NULL);

            if(x == -1)
            {
                printf("Exec error.\n");
                exit(1);
            }
        }

    }
    


}   