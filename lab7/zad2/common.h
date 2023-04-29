//
// Created by psp515 on 28/04/2023.
//

#ifndef LAB7_COMMON_H
#define LAB7_COMMON_H


#define PROJECT "SHAREDMEMORY"
#define SEM_QUEUE_FNAME "/sem_queue2"
#define SEM_CHAIRS_FNAME "/sem_chairs2"
#define SEM_BARBERS_FNAME "/sem_barbers2"
#define SEM_BUFFER_MUTEX_FNAME "/sem_mutex2"
#define SIZE 1024

#include <errno.h>

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

#endif //LAB7_COMMON_H
