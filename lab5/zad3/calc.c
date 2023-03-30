//
// Created by psp515 on 30/03/2023.
//
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>

double f(double x)
{
    return 4 / (x * x - 1);
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
    if(n != 5)
    {
        printf("Invalid number of args. (should be 4)");
    }

    double start = parse(args[2]);
    double stop = parse(args[3]);
    double interval = parse(args[4]);

    double area = integral(start, stop, interval);

    int descriptor = open(args[1], O_WRONLY);

    char buffer[PIPE_BUF];
    size_t size = snprintf(buffer, PIPE_BUF, "%lf", area);
    write(descriptor, buffer, size);

    close(descriptor);
    exit(0);
}