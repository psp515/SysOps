//
// Created by lukasz on 09.03.23.
//
#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1

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
    if(n!=5)
    {
        printf("Invalid number of arguments.");
        return -1;
    }

    if(strlen(args[1]) != 1 || strlen(args[2]) != 1)
    {
        printf("Invalid len of 1-st or 2-nd argument.");
        return -1;
    }

    if(access(args[3],F_OK)==-1)
    {
        printf("File to copy from does not exist.");
        return -1;
    }

    if(access(args[4],F_OK)!=-1)
    {
        printf("File to copy to exist(it should not because could override valid file).");
        return -1;
    }

    clock_t start, stop;
    struct tms tms_start, tms_stop;

    start = clock();
    times(&tms_start);


    FILE* readFrom = fopen(args[3],"r");

    if(readFrom== NULL)
    {
        printf("Error reading file.");
        return -1;
    }

    FILE* writeTo = fopen(args[4],"w");

    if(writeTo == NULL)
    {
        printf("Error writing file.");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead = fread(buffer,1,sizeof(buffer), readFrom);

    while(bytesRead > 0)
    {
        if (buffer[0] == args[1][0])
            fwrite(args[2], 1, sizeof(buffer),writeTo);
        else
            fwrite(buffer, 1, sizeof(buffer),writeTo);

        bytesRead = fread(buffer, 1, sizeof(buffer), readFrom);
    }

    fclose(readFrom);
    fclose(writeTo);

    stop = clock();
    times(&tms_stop);

    log_data("Times lib", tms_start, tms_stop, start, stop);
}