#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>

#define BUFF PIPE_BUF
#define FIFO_PATH "/tmp/fifo.txt"

long double calculate_time_clocks(clock_t start, clock_t end) {
    return (long double) (end - start) / CLOCKS_PER_SEC;
}

double parse(char* string)
{
    char *endptr;
    double value = strtod(string, &endptr);
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
    if(n != 3)
    {
        printf("Invalid number of args. (should be 3)");
    }

    double interval = parse(args[1]);

    char* endptr;
    int processes = strtol(args[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        printf("%s is not a valid number.\n", args[2]);
        return 1;
    }

    clock_t start, stop;
    start = clock();
    char buffer[BUFF];

    mkfifo(FIFO_PATH, 0666);

    for(int i = 0; i < processes; i++)
    {
        pid_t pid = fork();
        if(pid == -1)
        {
            perror("Fork error");
            exit(1);
        }
        else if(pid == 0)
        {
            char arg1[BUFF], arg2[BUFF],arg3[BUFF];
            double start = i * processes;
            double stop = (i+1) * processes;

            snprintf(arg1, BUFF, "%.15lf", start);
            snprintf(arg2, BUFF, "%.15lf", stop);
            snprintf(arg3, BUFF, "%.15lf", interval);

            execl("./calc", "calc", FIFO_PATH, arg1, arg2, arg3, NULL);
            printf("Exec error");
            exit(1);
        }
    }

    double sum = 0;

    // Already awaits for data

    int descriptor = open(FIFO_PATH, O_RDONLY);

    for(int i = 0; i < processes;)
    {
        memset(buffer, 0, sizeof(buffer));
        read(descriptor, buffer, BUFF);

        char *token = strtok(buffer, "\n");

        while (token != NULL)
        {
            sum += parse(token);
            token = strtok(NULL, " ");
            i++;
        }
    }

    close(descriptor);
    remove(FIFO_PATH);

    stop = clock();

    printf("---Integral data---\n");
    printf("rt: %.10Lf\n", calculate_time_clocks(start,stop));
    printf("Sum: %.10lf\n", sum);
    printf("Programs started: %d\n", processes);
    printf("Interval size: %.10lf\n", interval);

    exit(0);
}