#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define FIELDS 900
pthread_t* threads;
char *fg;
char *bg;

struct threadData
{
    int i;
    int j;
    int idx;
};

const int grid_width = 30;
const int grid_height = 30;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}
void destroy_grid(char *grid)
{
    free(grid);
    free(threads);
}
void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void* worker(void* arg)
{
    struct threadData *data = (struct threadData *) arg;
    //printf("Starting: %d, %d, %d\n", data->idx,data->i, data->j);
    //fflush(stdout);

    pause();

    while(1)
    {
        bg[data->idx] = is_alive(data->i, data->j, fg);
        pause();
    }

    return 0;
}
void pass(){}
void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;

    struct threadData *data = malloc(FIELDS* sizeof(struct threadData));
    threads = malloc(FIELDS * sizeof(pthread_t));

    // Set for all threads
    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = pass;
    sigaction(SIGUSR1, &sigact, NULL);

    for(int i = 0; i < grid_height; i++)
    {
        for(int j = 0; j < grid_width; j++)
        {
            int idx = i*grid_height + j;
            data[idx].i = i;
            data[idx].j = j;
            data[idx].idx = idx;

            pthread_t id;
            pthread_create(&id, NULL, worker, (void *) &data[idx]);
            threads[idx] = id;
        }
    }
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}
void update_grid(char *src, char *dst)
{
    fg = src;
    bg = dst;

    for (int i = 0; i < FIELDS; i++)
    {
        int error = pthread_kill(threads[i], SIGUSR1);
        if(error != 0)
        {
            perror("SIgnal sending error. Thread error.");
            exit(1);
        }
    }
}


