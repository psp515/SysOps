//
// Created by lukasz on 04.03.23.
//

#include "lib.h"
#include <unistd.h>

struct FileDataArray* createFileDataArray(int maxSize)
{
    struct FileDataArray* array = calloc(sizeof(FileDataArray), 1);

    if(array == NULL)
    {
        printf("Problems with allocation when allocating FileDataArray.")
        exit(ALLOCATION_ERROR);
    }

    array.filesData = calloc(sizeof(FileData), maxSize);

    if(array.filesData == NULL)
    {
        printf("Problems with allocation when allocating FilesData.")
        exit(ALLOCATION_ERROR);
    }

    array->maxDataLen = maxSize;
    array->dataLen = 0;

    return array;
}

bool countFileData(struct FileDataArray* array, char* fileName)
{
    if(array->dataLen == array->maxDataLen)
        return false;

    if (access(fileName, F_OK) == -1)
        return false;

    //TODO dir check

    //TODO file check

    //TODO save to file

    //TODO read from file

    //GOON OON

    system("wc "+ fileName+" >> tmp")





    return true;
}


struct FileData getFileData(struct FileDataArray* array, int index){
    if(index < 0 || index > array.dataLen)
        return NULL;

    return array->filesData[index];
}

void deleteFileData(struct FileDataArray* array, int index){
    if(index < 0 || index > array.dataLen)
        return;

    for(int i = index; i < array.dataLen; i++)
        array[i] = array[i+1];

    array.dataLen--;
}

void freeFileDataArray(struct FileDataArray* array){
    free(array->filesData);
    free(array)
}

