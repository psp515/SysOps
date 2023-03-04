//
// Created by lukasz on 04.03.23.
//

#include <stdlib.h>
#include <stdio.h>

#define OTHER_ERROR 0
#define ALLOCATION_ERROR 1


struct FileData{
    int lines;
    int words;
    int characters;
} fileData;

struct FileDataArray{
    struct FileData* filesData;
    int maxDataLen;
    int dataLen;
} fileDataArray;