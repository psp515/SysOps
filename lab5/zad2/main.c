#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

#define BUFF 1024

double f(double x)
{
    return 4 / (x * x - 1);
}

long double calculate_time_clocks(clock_t start, clock_t end) {
    return (long double) (end - start) / CLOCKS_PER_SEC;
}

double integral(double start, double stop, double interval)
{
    double sum = 0;

    for(double x = start; x < stop;)
    {
        sum += interval * f(x);
        x += interval;
    }

    return sum;
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
    int descriptors[processes][2];

    for(int i = 0; i < processes; i++)
    {
        if(pipe(descriptors[i]) == -1)
        {
            perror("Pipe error");
            exit(1);
        }

        pid_t pid = fork();
        if(pid == -1)
        {
            perror("Fork error");
            exit(1);
        }
        else if(pid == 0)
        {
            close(descriptors[i][0]);
            double start = i * processes;
            double stop = (i+1) * processes;
            double area = integral(start, stop, interval);

            memset(buffer, 0, sizeof(buffer));
            size_t written_size = snprintf(buffer, BUFF, "%f", area);
            write(descriptors[i][1],buffer, written_size);

            exit(0);
        }
        else
            close(descriptors[i][1]);

    }

    while(wait(NULL) > 0){}

    double sum = 0;

    for(int i = 0; i < processes; i++)
    {
        memset(buffer, 0, sizeof(buffer));
        read(descriptors[i][0], buffer, BUFF);
        sum += parse(buffer);
    }

    stop = clock();

    printf("---Integral data---\n");
    printf("rt: %.10Lf\n", calculate_time_clocks(start,stop));
    printf("Sum: %.10lf\n", sum);
    printf("Processes: %d\n", processes);
    printf("Interval size: %.10lf\n", interval);

    exit(0);
}