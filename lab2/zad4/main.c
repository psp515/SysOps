//
// Created by lukasz on 10.03.23.
//

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    DIR* dir = opendir(".");
    struct dirent *dir_element = readdir(dir);

    long long total_bytes = 0;


    while (dir_element != NULL)
    {
        struct stat data;
        int state = stat(dir_element->d_name, &data);

        if(state == -1)
        {
            printf("Error reading stat. File %s.", dir_element->d_name);
            return -1;
        }

        if(!S_ISDIR(data.st_mode))
        {
            total_bytes += data.st_size;
            printf("%ld %s\n", data.st_size, dir_element->d_name);
        }

        dir_element = readdir(dir);
    }


    printf("Total bytes: %lld \n", total_bytes);
    closedir(dir);
}