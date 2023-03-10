//
// Created by lukasz on 09.03.23.
//
#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

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

void reverse(char* buff, long size)
{
    char *start = buff;
    char *end = buff + size - 1;

    while (start < end)
    {
        char tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }
}

int main(int n, char** args)
{
    /*Ten sam program dla bloku wielkosci 1 i 1024 ale inne parametry wywołania!! */

    if(n != 4)
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
        printf("File to reverse to exist(it should not because program could override valid file).");
        return -1;
    }

    char* end;
    long block_size = strtol(args[3],&end,10);

    if((*end != '\0' || isspace(*end)) && (block_size < 0 || block_size > 2049))
    {
        printf("Invalid block size.(should be 0 < block < 2049)");
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

    start = clock();
    times(&tms_start);

    fseek(readFrom, 0L, SEEK_END);
    long size = ftell(readFrom);
    fseek(readFrom, 0L, SEEK_SET);

    char buffer[block_size];

    for(long i = size - block_size; i >= 0; i -= block_size) {
        fseek(readFrom, i, SEEK_SET);
        fread(buffer, sizeof(char), block_size, readFrom);
        reverse(buffer, block_size);
        fwrite(buffer, sizeof(char), block_size, writeTo);
    }

    if(size % block_size != 0) {
        fseek(readFrom, 0, SEEK_SET);
        fread(buffer, sizeof(char), size % block_size, readFrom);
        reverse(buffer, size);
        fwrite(buffer, sizeof(char), size % block_size, writeTo);
    }


    stop = clock();
    times(&tms_stop);

    fclose(readFrom);
    fclose(writeTo);

    log_data("Times lib", tms_start, tms_stop, start, stop);
}