#include <string.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>


int main(int n, char** args){

    if(n!=2)
    {
        printf("Invalid number of parameters. (Should be 2)");
        exit(-1);
    }
    struct stat st;

    if(stat(args[1], &st) != 0 || !S_ISDIR(st.st_mode))
    {
        printf("Route to directory should be first argument.");
        exit(-1);
    }

    printf("%s", args[0]);
    // bez fflush naza jest printowana 2 razy 
    fflush(stdout);
    __pid_t pid = fork();

    if(pid == 0)
    {
        if(execl("/bin/ls", "ls", args[1], NULL) == -1)
        {
            printf("Execl error.");
            exit(EXIT_FAILURE);
        }
    }
   
}