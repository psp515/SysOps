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

void save_reverse_in_file(FILE* writeTo, char* buff, int buff_size)
{
    printf("%s", buff);
    /*for(int i = buff_size-1;i >-1;i++)
    {

        //write by 1 char (in instruction it is specified only about reading file)
        //char tmp[1];
        //tmp[0] =  buff[i];
        //fwrite(tmp, 1, 1,writeTo);
    }*/
}


int main(int n, char** args)
{
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
        printf("File to reverse to exist(it should not because could override valid file).");
        return -1;
    }

    char* end;
    int block_size = strtol(args[3],&end,10);

    if((*end != '\0' || isspace(*end)) && (block_size < 0 || block_size > 2048))
    {
        printf("Invalid block size.");
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
    char buffer[block_size];

    fseek(readFrom, -1L, SEEK_END);

    while(size > 0)
    {
        if(size < block_size)
        {
            fseek(readFrom, 0L, SEEK_SET);
            fread(buffer,1,size, readFrom);

            printf("%s\n", buffer);

            save_reverse_in_file(writeTo, buffer,size);

            size = 0;
        }
        else
        {
            long block = -2 * block_size;
            fseek(readFrom, block, SEEK_CUR);
            size_t bytesRead = fread(buffer,1,sizeof(buffer), readFrom);
            size -= bytesRead;

            if(bytesRead == 0)
                break;

            save_reverse_in_file(writeTo, buffer,block_size);
        }
    }


    stop = clock();
    times(&tms_stop);

    fclose(readFrom);
    fclose(writeTo);

    log_data("Times lib", tms_start, tms_stop, start, stop);
}