#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_ELVES 10
#define MAX_REINDEERS 9

typedef struct {
    int id;
} Identifier;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santa_wake_up_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t problems_solved_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t delivery_cond = PTHREAD_COND_INITIALIZER;

int deliveries = 0;
int elfs[3];
int elfs_waiting = 0;
int raindeers_ready = 0;

int randint(int min, int max)
{
    srand(time(NULL));
    return rand() % (max - min + 1) + min;
}

void* santa(void* args)
{
    Identifier ident = *(Identifier *)args;
    int id = ident.id;

    printf("[Santa] Santa is alive.\n");
    fflush(stdout);

    while(1)
    {
        pthread_mutex_lock(&mutex);

        while(elfs_waiting < 3 && raindeers_ready < 9)
            pthread_cond_wait(&santa_wake_up_cond, &mutex);
        
        printf("[Santa] Woke up.\n");
        fflush(stdout);

        if(elfs_waiting == 3)
        {
            printf("[Santa] Solving elfs problems %d, %d, %d.\n", elfs[0], elfs[1], elfs[2]);
            fflush(stdout);

            sleep(randint(1, 2));

            elfs_waiting = 0;
            pthread_cond_broadcast(&problems_solved_cond);
        }

        if(raindeers_ready == 9)
        {
            printf("[Santa] Delivering presents.\n");
            fflush(stdout);

            sleep(randint(2, 4));

            raindeers_ready = 0;
            pthread_cond_broadcast(&delivery_cond);
            deliveries++;
        }

        if(deliveries >= 3)
            break;

        printf("[Santa] Goes to sleep.\n");
        fflush(stdout);

        pthread_mutex_unlock(&mutex);
    }

    printf("[Santa] Santa is going on rent.\n");

    exit(0);
}

void* reindeer(void* args)
{    
    Identifier ident = *(Identifier *)args;
    int id = pthread_self();
    
    printf("[Reindeer-%d] Reindeer is alive.\n", id);
    fflush(stdout);

    while(1)
    {
        sleep(randint(5, 10));

        pthread_mutex_lock(&mutex);

        raindeers_ready++;
        printf("[Reindeer-%d] Reindeer is back from ibiza. Awaiting raindeers %d\n", id, raindeers_ready);
        fflush(stdout);

        if(raindeers_ready == 9)
        {
            printf("[Reindeer-%d] Last reindeer is back. Waking up santa.\n", id);
            pthread_cond_signal(&santa_wake_up_cond);
        }

        pthread_cond_wait(&delivery_cond, &mutex);

        pthread_mutex_unlock(&mutex);

        pthread_testcancel();
    }

    exit(0);
}

void* elf(void* args)
{
    Identifier ident = *(Identifier *)args;
    int id = pthread_self();

    printf("[Elf-%d] Elf is alive.\n", id);
    fflush(stdout);

    while(1)
    {
        sleep(randint(2, 5));

        pthread_mutex_lock(&mutex);

        if(elfs_waiting == 3)
        {
            printf("[Elf-%d] Elf sees the queue and decides to solve problem by his own.\n", id); 
            fflush(stdout);
            sleep(randint(1, 2));   
        }
        else
        {
            elfs[elfs_waiting] = id;
            elfs_waiting++;

            printf("[Elf-%d] Elf has a problem.\n", id);
            fflush(stdout);

            if(elfs_waiting == 3)
            {
                printf("[Elf-%d] Third elf problem. Waking up santa.\n", id);
                pthread_cond_signal(&santa_wake_up_cond);
            }

            pthread_cond_wait(&problems_solved_cond, &mutex);
            
            printf("[Elf-%d] Santa solved my problem.\n", id);
            fflush(stdout);
        }

        pthread_mutex_unlock(&mutex);

        pthread_testcancel();
    }
    
    exit(0);
}

pthread_t create_thread(int id, void* (*actions)(void*))
{
    pthread_t thread;
    Identifier ident = {id};
    if(pthread_create(&thread, NULL, actions, (void *)&ident))
    {
        printf("[Simulation] Error creating thread for %d.\n", id);
        exit(-1);
    }

    return thread;
}

int main() 
{
    pthread_t threads[20];

    printf("[SIMULATION] Starting simulation.\n");
    fflush(stdout);
    
    threads[0] = create_thread(0, santa);

    for(int i = 0; i < MAX_ELVES; i++)
        threads[1 + i] = create_thread(i, elf);

    for(int i = 0; i < MAX_REINDEERS; i++)
        threads[11 + i] = create_thread(i, reindeer);
        
    pthread_join(threads[0], NULL);

    for(int i = 1; i < 20; i++)
        pthread_cancel(threads[i]);

    for(int i = 1; i < 20; i++)
        pthread_join(threads[i], NULL);
    
    printf("[SIMULATION] Simulation ended.\n");
    fflush(stdout);

    return 0;
}

