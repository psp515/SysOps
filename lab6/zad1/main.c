#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>
#include "helper.h"
#include "time.h"

key_t clientsKeys[MAXCLIENTS];
int newId;
int serverQueue;
long SIZE;

void save(struct msgData* message)
{
    FILE* logs = fopen("logs.txt", "w");
    char data[2*MSGMAXLEN];

    switch(message->mtype){
        case ONE:
            snprintf(data, sizeof (data),"Type: %ld From id: %d To id: %d date: %s data: %s\n",message->mtype, message->client_id, message->to_id, message->date, message->buffer);
            fwrite(data,sizeof (char ),sizeof (data),logs);
            break;
        case ALL:
            snprintf(data, sizeof (data),"Type: %ld From id: %d date: %s data: %s\n",message->mtype, message->client_id, message->date, message->buffer);
            fwrite(data,sizeof (char ),sizeof (data),logs);
            break;
        default:
            snprintf(data, sizeof (data),"Type: %ld From id: %d date: %s\n",message->mtype, message->client_id, message->date);
            fwrite(data,sizeof (char ),sizeof (data),logs);
    }

    fclose(logs);
}

int parse(char* string)
{
    char *endptr;
    int value = strtod(string, &endptr);
    if (errno == ERANGE) {
        printf("Value out of range for double\n");
        exit(1);
    }
    else if (*endptr != '\0') {
        printf("Invalid input: '%s'\n", endptr);
        exit(1);
    }
    return value;
}

void handleAll(struct msgData* buff)
{
    for(int i = 0; i < MAXCLIENTS; i++)
    {
        if(clientsKeys[i] != -1 && buff->client_id != i)
        {
            printf("---%d---\n", i);
            int tmpQueue = msgget(clientsKeys[i], 0);
            msgsnd(tmpQueue, buff, SIZE, 0);
        }
    }
}

void handleOne(struct msgData* buff)
{
    int toId = buff->to_id;
    printf("---%d---\n", toId);
    if(clientsKeys[toId] != -1)
    {
        int tmpQueue = msgget(clientsKeys[toId], 0);
        msgsnd(tmpQueue, buff, SIZE, 0);
    }
}

void handleList(int newId)
{
    for(int i = 0;i < newId; i++)
        if(clientsKeys[i] != -1)
            printf("id: %d, q_key: %d\n", i, clientsKeys[i]);
}

void handleStop(struct msgData* buff)
{
    int id = buff->client_id;

    if(id > MAXCLIENTS || clientsKeys[id] == -1)
        return;

    clientsKeys[id] = -1;
}

void handleInit(struct msgData* buff)
{
    int clientQueue = buff->new_client_queue;
    struct msgData* message = malloc(sizeof (msgData));
    int tmpQueue = msgget(clientQueue, 0);
    message->mtype = INIT;
    if(newId >= MAXCLIENTS)
    {
        message->client_id = -1;
        msgsnd(tmpQueue, buff, SIZE, 0);
        free(message);
        return;
    }

    clientsKeys[newId] = clientQueue;
    message->client_id = newId;
    printf("--%d --\n", newId);
    msgsnd(tmpQueue, message, SIZE, IPC_NOWAIT);
    free(message);
    newId += 1;
    printf("--%d --\n", newId);
}

void stopServer()
{
    struct msgData *message = malloc(sizeof (msgData));
    message->mtype = SERVER_DOWN;
    message->client_id = -1;
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);
    strftime(message->date, 19, "%Y-%m-%d", local_time);

    for(int i = 0; i < newId; i++)
    {
        if(clientsKeys[i] != -1)
        {
            int queue = msgget(clientsKeys[i], 0);
            message->to_id = i;
            msgsnd(queue, message, SIZE, 0);
            //msgrcv(serverQueue, message, SIZE, -1, 0);

            save(message);
        }
    }

    msgctl(serverQueue, IPC_RMID, NULL);
}
void exitHandler()
{
    printf("Server shut down\n");
    exit(0);
}

int main()
{
    int key = ftok(PATH, 1);
    serverQueue = msgget(key, IPC_CREAT | 0666);
    printf("Server key: %d\n", key);

    atexit(stopServer);
    signal(SIGINT, exitHandler);

    struct msgData *data = malloc(sizeof (msgData));
    SIZE = sizeof (msgData) - sizeof (long);
    newId = FIRST_ID;

    for(int i =0; i < MAXCLIENTS; i++)
        clientsKeys[i] = -1;

    printf("Server started\n");

    while(1)
    {
        msgrcv(serverQueue, data, SIZE, -6, 0);

        switch(data->mtype){
            case STOP:
                printf("Handling STOP\n");
                handleStop(data);
                save(data);
                break;
            case INIT:
                printf("Handling INIT\n");
                handleInit(data);
                save(data);
                break;
            case LIST:
                printf("Handling LIST\n");
                handleList(newId);
                save(data);
                break;
            case ONE:
                printf("Handling ONE\n");
                handleOne(data);
                save(data);
                break;
            case ALL:
                printf("Handling ALL\n");
                handleAll(data);
                save(data);
                break;
        }
    }
}