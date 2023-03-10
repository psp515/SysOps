//
// Created by lukasz on 09.03.23.
//

#include <stdio.h>

#define SMALL_BLOCK_SIZE 1

double calculate_time_clocks(clock_t start, clock_t end) {
    return (double) (end - start) / CLOCKS_PER_SEC;
}

double calculate_time_tics(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void log_data(char* operation,
              struct tms tms_start,
              struct tms tms_stop,
              clock_t start,
              clock_t stop)
{
    printf("---%s---\n", operation);
    printf("rt: %f\n", calculate_time_clocks(start,stop));
    printf("ut: %f\n", calculate_time_tics(tms_start.tms_utime,tms_stop.tms_utime));
    printf("st: %f\n", calculate_time_tics(tms_start.tms_stime,tms_stop.tms_stime));
}


int main(int n, char** args)
{
    if(n!=3)
    {
        printf("Invalid number of arguments.");
        return -1;
    }

    if(access(args[1], F_OK) == -1)
    {
        printf("File to copy from does not exist.");
        return -1;
    }

    if(access(args[2], F_OK) != -1)
    {
        printf("File to reverse to exist(it should not because could override valid file).");
        return -1;
    }

    clock_t start, stop;
    struct tms tms_start, tms_stop;

    FILE* readFrom = fopen(args[1], "r");

    if(readFrom== NULL)
    {
        printf("Error reading file.");
        return -1;
    }

    FILE* writeTo = fopen(args[2], "w");

    if(writeTo == NULL)
    {
        printf("Error writing file.");
        return -1;
    }


    log_data("Times lib", tms_start, tms_stop, start, stop);
}