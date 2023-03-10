//
// Created by lukasz on 10.03.23.
//

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ftw.h>

long long bytes = 0;

int process(const char* path, const struct stat* data, int flag)
{
    if(!S_ISDIR(data->st_mode) && flag == FTW_F)
    {
        bytes += data->st_size;
        printf("%ld %s\n", data->st_size, path);
    }

    return 0;
}

int main(int n, char** args) {
    if (n != 2) {
        printf("Invalid number of arguments. (should be 2)");
        return -1;
    }

    // zrozumiałem treść ze sprawdamy wszystko dlatego 0 (nieograniczona głębokosc do sprawdzenia)

    if(ftw(args[1],process,0) == -1)
    {
        printf("FTW Error.(probably invalid path)");
        return -1;
    }

    printf("Total bytes: %lld \n", bytes);
}
