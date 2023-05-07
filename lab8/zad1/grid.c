#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>

#define FIELDS 900

const int grid_width = 30;
const int grid_height = 30;

static int threads_finished = 0;
static int calculate = 0;
pthread_t* threads;
int* thread_calculated;

static char* internal_src;
static char* internal_dst;

struct threadData
{
    int i;
    int j;
} ThreadData;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
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

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
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
    internal_src = src;
    internal_dst = dst;
    threads_finished = 0;
    calculate = 1;

    while(threads_finished != grid_height* grid_width);
}

void* thread_work(struct threadData data)
{
    int idx = data.i * grid_width + data.j;

    while(1)
    {
        if(calculate == 0)
        {
            thread_calculated[1] = 0;
            continue;
        }
        
        if(thread_calculated[1] == 1)
            continue;
        
        internal_dst[idx] = is_alive(data.i, data.j, internal_src);
        thread_calculated[1] = 1;
        threads_finished++;
    }
}

void create_threads()
{
    threads = malloc(sizeof(pthread_t) * FIELDS);
    thread_calculated = malloc(sizeof(int) * FIELDS);

    for(int i = 0; i < grid_height; i++)
        for(int j = 0; j < grid_width; j++)
        {
            struct threadData data = {i, j};
            pthread_t thread;
            pthread_create(&thread, NULL, thread_work, (void *) data);
            threads[i*grid_height + j] = thread;
        }
}
