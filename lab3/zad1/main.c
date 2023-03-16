#include <string.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>

int isValidNumber(char* string)
{
    for(int i = 0; i < strlen(string); i++)
        if (isdigit(string[i]) == 0)
            return 1;

    return 0;
}


int main(int n, char** args){

    if(n!=2)
    {
        printf("Invalid number of parameters. (Should be 2)");
        exit(-1);
    }

    if(isValidNumber(args[1]))
    {
        printf("Invalid parameter. (Should be number)");
        exit(-2);
    }

    int count = atoi(args[1]);

    __pid_t root_id = getpid();

    for (size_t i = 0; i < count; )
    {
        if(getpid() == root_id)
        {
            __pid_t x = fork();
            i++;

            if (x == -1)
            {
                perror("fork");
                exit(-3);
            }
            else if (i == count && getpid() != root_id)
            {
                printf("Proces: %d %d\n", getpid(), getppid());
                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            printf("Proces: %d %d\n", getpid(), getppid());
            exit(EXIT_SUCCESS);
        }
    }
    
   for(int i = 0; i < count; i++)
   {  
        int status;
        pid_t pid = wait(&status);
        if(pid == -1) 
        {
            perror("Wait error.");
            break;
        }
        
   }


    printf("%d\n", count);

}