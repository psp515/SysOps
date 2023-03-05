//
// Created by lukasz on 04.03.23.
//

#include "lib.h"
#include <unistd.h>
#include <sys/stat.h>
#include "string.h"

struct FileDataArray* createFileDataArray(int maxSize)
{
    struct FileDataArray* array = calloc(sizeof(fileDataArray), 1);

    if(array == NULL)
    {
        printf("Problems with allocation when allocating FileDataArray.");
        exit(ALLOCATION_ERROR);
    }

    array->filesData = calloc(sizeof(fileData), maxSize);

    if(array->filesData == NULL)
    {
        printf("Problems with allocation when allocating FilesData.");
        exit(ALLOCATION_ERROR);
    }

    array->maxDataLen = maxSize;
    array->dataLen = 0;

    return array;
}

bool countFileData(struct FileDataArray* array, char* fileName)
{
    // array is full
    if(array->dataLen == array->maxDataLen)
        return false;

    // file doesn't exist
    if (access(fileName, F_OK) == -1)
        return false;

    // dir exist
    struct stat st;
    if (stat("tmp", &st) != 0)
        mkdir("tmp",0777);

    int filenameLen = strlen(fileName);
    char tmp[filenameLen + 8];
    sprintf(tmp, "tmp/tmp%s", fileName);

    char command[2*filenameLen + 15];
    sprintf(command, "wc %s >> %s", fileName, tmp);

    system(command);

    FILE* file = fopen(tmp,"r");

    if(file == NULL)
        return false;

    struct FileData data = {0,0,0};

    fscanf(file,"%d %d %d", &data.lines, &data.words, &data.characters);

    array->filesData[array->dataLen] = data;
    array->dataLen++;

    fclose(file);

    remove(tmp);

    return true;
}


struct FileData getFileData(struct FileDataArray* array, int index){
    if(index < 0 || index > array->dataLen)
    {
        struct FileData data ={-1,-1,-1};
        return data;
    }

    return array->filesData[index];
}

void deleteFileData(struct FileDataArray* array, int index){
    if(index < 0 || index > array->dataLen)
        return;

    for(int i = index; i < array->dataLen; i++)
        array->filesData[i] = array->filesData[i+1];

    array->dataLen--;
}

void freeFileDataArray(struct FileDataArray* array){
    free(array->filesData);
    free(array);
}

int main(){

    struct FileDataArray* array = createFileDataArray(10);

    countFileData(array, "lib.h");
    countFileData(array, "lib.c");

    printf("%d\n", array->dataLen);

    struct FileData x = getFileData(array, 0);
    printf("%d %d %d \n", x.lines,x.words,x.characters);

    x = getFileData(array, 1);
    printf("%d %d %d \n", x.lines, x.words, x.characters);

    deleteFileData(array, 0);

    printf("%d\n", array->dataLen);

    x = getFileData(array, 0);
    printf("%d %d %d \n", x.lines, x.words, x.characters);

    //Działa jajks
}