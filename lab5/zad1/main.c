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
            snprintf(command, BUFSIZ, "mail -H | sort -k 6");
        else if(strcmp(args[1], "nadawca") == 0)
            snprintf(command, BUFSIZ, "mail -H | sort -k 3");
        else
        {
            printf("Inalid argument should be data or nadawca.\n");
            exit(-1);
        }

        FILE* mails = popen(command, "w");

        fputs("exit", mails);
        pclose(mails);
    }
    else if(n == 4)
    {
        char buffer[BUFSIZ];
        snprintf(buffer, BUFSIZ, "mail -s %s %s", args[2], args[1]);
        FILE* new_mail = popen(buffer, "w");
        fputs(args[3], new_mail);
        pclose(new_mail);
    }
    else
    {
        printf("Invalid number of arguments (should be 1 or 3)\n");
        exit(-1);
    }
}