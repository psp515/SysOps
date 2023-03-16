#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>

#define MAX_PATTER_LEN 256

void check_directory(char * dir_path, char* pattern)
{
    DIR* act_dir = opendir(dir_path);

    if(act_dir == NULL)
    {
        perror("Error opening folder");
        exit(1);
    }

    struct dirent *dir_element = readdir(act_dir);
    struct stat data;

    while (dir_element != NULL)
    {
        char new_path[PATH_MAX];
        snprintf(new_path, PATH_MAX, "%s/%s", dir_path, dir_element->d_name);
        int status = stat(new_path, &data);

        if(status == -1)
        {
            perror("Error reading file");
            exit(1);
        }
        else if(S_ISDIR(data.st_mode))
        {
            if (strcmp(dir_element->d_name, ".") == 0 || strcmp(dir_element->d_name, "..") == 0)
            {
                dir_element = readdir(act_dir);
                continue;
            }

            pid_t process_id = fork();
            if(process_id == 0)
            {
                check_directory(new_path, pattern);
                exit(0);
            }
        }
        else if(S_ISREG(data.st_mode))
        {
            FILE* file = fopen(new_path, "r");

            if (file == NULL)
            {
                perror("Fopen error");
                exit(3);
            }

            char buff[MAX_PATTER_LEN];

            fread(buff, 1, sizeof(buff), file);

            if(strncmp(pattern, buff, strlen(pattern)) == 0)
            {
                printf("Route: %s, pid: %d\n", new_path, getpid());
                fflush(NULL);
            }

            fclose(file);
        }

        dir_element = readdir(act_dir);
    }

    closedir(act_dir);
}

int main(int n, char** args){
    
    if(n!=3)
    {
        printf("Invalid number of parameters. (Should be 3 got %d)",n);
        exit(-1);
    }
    struct stat st;

    if(stat(args[1], &st) != 0 || !S_ISDIR(st.st_mode))
    {
        printf("Valid route to directory should be first argument got %s", args[1]);
        exit(-1);
    }

    if(strlen(args[2]) > 255)
    {
        printf("Second argument should be shorter than 255 chars.");
        exit(-1);
    }
    
    check_directory(args[1], args[2]);
}