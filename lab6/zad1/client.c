#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>



void exitHandler()
{

    exit(0);
}

int parse(char* string)
{
    char *endptr;
    int value = strtod(string, &endptr);
    if (errno == ERANGE) {
        printf("Value out of range for double\n");
        exit(1);
    }
    else if (*endptr != '\0') {
        printf("Invalid input: '%s'\n", endptr);
        exit(1);
    }
    return value;
}

int main(int n, char** args)
{
    if(n != 2)
    {
        printf("Invalid number of arguments");
        exit(1);
    }

    int key = parse(args[1]);
    int queue = msgget(key, IPC_PRIVATE);

    signal(SIGINT, exitHandler);

    while(1);

    exit(0);
}