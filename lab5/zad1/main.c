#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 1024

int main(int n, char** args)
{
    if(n == 2)
    {
        char command[MAX_LINE_LENGTH];
        if(strcmp(args[1], "data") == 0)
        {
            snprintf(command, MAX_LINE_LENGTH, "mail -H | sort -k 2n | less");
        }
        else if(strcmp(args[1], "nadawca") == 0)
        {
            snprintf(command, MAX_LINE_LENGTH, "mail -H | sort -k 2M | less");
        }
        else
        {
            printf("Inalid argument should be data or nadawca.\n");
            exit(-1);
        }

        FILE* mails = popen("", "r");

        pclose(mails);
    }
    else if(n == 4)
    {

    }
    else
    {
        printf("Invalid number of arguments (should be 1 or 3)\n");
        exit(-1);
    }
}