#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>


int main(int n, char* args[]) {

    if(n != 5)
    {
        printf("---- Invalid number of arguemnts. [SIMULATION ERROR] ----\n");
        fflush(stdout);
        return 1;
    }

}

