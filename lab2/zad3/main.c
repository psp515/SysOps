//
// Created by lukasz on 10.03.23.
//

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main()
{
    DIR* dir = opendir(".");
    struct dirent *dir_element = readdir(dir);

    long long total_bytes = 0;


    while (dir_element != NULL)
    {
        // folder zaczynaja sie od .
        if(dir_element->d_name[0] != '.')
        {
            struct stat data;
            stat(dir_element->d_name, &data);

            printf("", data.)
        }
        dir_element = readdir(dir);
    }

    closedir(dir);

}