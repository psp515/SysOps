#include <signal.h>
#include <stdio.h>

void is_pending(){

    sigset_t pending;
    sigpending(&pending);
    if (sigismember(&pending, SIGUSR1)) 
        printf("Pending.\n");
    else 
        printf("Not pending.\n");
        
}

int main(int argc, char** argv)
{
    printf("Exec");
    is_pending();

    raise(SIGUSR1);
    
    is_pending();

    return 0;
}