#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>


void check_directory(char * dir_path, char* pattern)
{
    DIR* act_dir = opendir(dir_path);

    if(act_dir == NULL)
    {
        perror("Error opening folder");
        exit(1);
    }

    struct dirent *dir_element = readdir(act_dir);

    while (dir_element != NULL)
    {

        struct stat data;
        int status = stat(dir_element->d_name, &data);

        int n = strlen(dir_path) + strlen(dir_element->d_name);
        char new_path[n];
        snprintf(new_path, n, "%s/%s", dir_path, dir_element->d_name);

        if(status == -1)
        {
            perror("Error reading file");
            printf(dir_path);
            printf(dir_element->d_name);
            exit(1);
        }
        else if(S_ISDIR(data.st_mode))
        {
            if (strcmp(dir_element->d_name, ".") == 0 || strcmp(dir_element->d_name, "..") == 0) 
                continue;

            __pid_t process_id = fork();

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

            char buff[strlen(pattern)];

            fread(buff, 1, sizeof(buff), file);

            if(strcmp(pattern, buff) == 0)
            {
                printf("Route: %s, pid: %d\n", new_path, getpid());
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
        printf("Invalid number of parameters. (Should be 3)");
        exit(-1);
    }
    struct stat st;

    if(stat(args[1], &st) != 0 || !S_ISDIR(st.st_mode))
    {
        printf("Route to directory should be first argument.");
        exit(-1);
    }

    if(strlen(args[2]) > 255)
    {
        printf("Second argument should be shorter than 255 chars.");
        exit(-1);
    }
    
    check_directory(args[1], args[2]);
}