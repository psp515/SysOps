#ifndef lib
#define lib

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define OTHER_ERROR 0
#define ALLOCATION_ERROR 1


typedef struct FileData{
    int lines;
    int words;
    int characters;
} fileData;

typedef struct FileDataArray{
    struct FileData* filesData;
    int maxDataLen;
    int dataLen;
} fileDataArray;

struct FileDataArray* createFileDataArray(int maxSize);

bool countFileData(struct FileDataArray* array, char* fileName);

struct FileData getFileData(struct FileDataArray* array, int index);

void deleteFileData(struct FileDataArray* array, int index);

void freeFileDataArray(struct FileDataArray* array);

#endif

