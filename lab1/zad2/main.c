//
// Created by lukasz on 05.03.23.
//

#include <stdio.h>
#include <stdbool.h>
#include <sys/times.h>
#include "lib.h"
#include <time.h>
#include <unistd.h>
#include "string.h"

#ifdef dynamic
#include <dlfcn.h>
#endif

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

int main()
{
    #ifdef dynamic
    void* handle = dlopen("./diff.so",RTLD_NOW);
    if(!handle)
    {
        fprintf(stderr, "%s", dlerror());
        exit(1);
    }

    fileDataArray* (*createFileDataArray)(int) = dlsym(handle,"createFileDataArray");
    bool (*countFileData)(fileDataArray*, char*) = dlsym(handle,"countFileData");
    fileData (*getFileData)(fileDataArray*, int) = dlsym(handle,"getFileData");
    void (*deleteFileData)(fileDataArray*, int) = dlsym(handle,"deleteFileData");
    void (*freeFileDataArray)(fileDataArray*) = dlsym(handle,"freeFileDataArray");

    #endif

    char buffer[256];

    char operation[256];
    int a = -1;

    bool initialized = false;
    struct FileDataArray* array;

    clock_t start, stop;
    struct tms tms_start, tms_stop;

    while (1) {

        fgets(buffer,256,stdin);

        if(sscanf(buffer,"init %d\n", &a) == 1)
        {
            start = clock();
            times(&tms_start);

            array = createFileDataArray(a);

            stop = clock();
            times(&tms_stop);

            log_data("create", tms_start,tms_stop,start, stop);

            initialized = true;
        }
        else if(sscanf(buffer,"count %s\n", operation) == 1 && initialized)
        {
            start = clock();
            times(&tms_start);

            countFileData(array, operation);

            stop = clock();
            times(&tms_stop);

            log_data("count", tms_start,tms_stop,start, stop);
        }
        else if(sscanf(buffer,"show %d\n", &a) == 1 && initialized)
        {
            start = clock();
            times(&tms_start);

            struct FileData data = getFileData(array, a);
            printf("i: %d, l: %d, w: %d, ch: %d\n", a, data.lines, data.words, data.characters);

            stop = clock();
            times(&tms_stop);

            log_data("show", tms_start,tms_stop,start, stop);
        }
        else if(sscanf(buffer,"delete index %d\n", &a) == 1 && initialized)
        {
            start = clock();
            times(&tms_start);

            deleteFileData(array, a);

            stop = clock();
            times(&tms_stop);

            log_data("delete", tms_start,tms_stop,start, stop);
        }
        else if(sscanf(buffer,"%s\n", operation) == 1 && strcmp("destroy", operation) == 0 && initialized)
        {
            start = clock();
            times(&tms_start);

            freeFileDataArray(array);

            stop = clock();
            times(&tms_stop);

            log_data("destroy", tms_start,tms_stop,start, stop);

            break;
        }
        else
        {
            printf("---- bad command ----\n");
        }
    }

    exit(0);
}