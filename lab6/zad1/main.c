#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>
#include "helper.h"

client* clients;
int newId;
int serverQueue;
long SIZE;

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

void handleAll(client* clients, struct msgData* buff)
{
    for(int i = 0; i < newId; i++)
    {
        if(clients[i].status)
        {
            int queue = msgget(clients[i].queueKey, 0);
            msgsnd(queue, buff, SIZE, 0);
        }
    }
}

void handleOne(client* clients, int newId, struct msgData* buff)
{
    int toId = buff->to_id;
    for(int i = 0; i < newId; i++)
    {
        if(clients[i].clientID == toId && clients[i].status)
        {
            int queue = msgget(clients[i].queueKey, 0);
            msgsnd(queue, buff, SIZE, 0);
            return;
        }
    }
}

void handleList(client* clients, int newId)
{
    for(int i = 0;i < newId; i++)
        printf("id: %d, q_key: %d, status: %d\n", clients->clientID, clients->queueKey, clients->status);
}

void handleStop(client* clients, struct msgData* buff)
{
    int id = buff->client_id;

    if(id > MAXCLIENTS && clients[id].status != -1)
        return;

    clients[id].status = 0;
}

void handleInit( client* clients, struct msgData* buff)
{
    int clientQueue = buff->new_client_queue;
    if(newId >= MAXCLIENTS)
    {
        struct msgData* message = malloc(sizeof (msgData));

        message->mtype = INIT;
        message->client_id = -1;
        int queue = msgget(clientQueue, 0);
        msgsnd(queue, buff, SIZE, 0);
    }

    client newClient = {newId, clientQueue,1};
    clients[newId] = newClient;

    struct msgData* message = malloc(sizeof (msgData));

    message->mtype = INIT;
    message->client_id = newId;
    newId += 1;
    int queue = msgget(clientQueue, 0);
    msgsnd(queue, message, SIZE, IPC_NOWAIT);
}

void exitHandler()
{
    struct msgData *message = malloc(sizeof (msgData));
    for(int i = 0; i < newId; i++)
    {
        if(clients[i].status)
        {
            message->mtype = SERVER_DOWN;
            int queue = msgget(clients[i].queueKey, 0);
            msgsnd(queue, message, SIZE, 0);
            msgrcv(serverQueue, message, SIZE, STOP, 0);
        }
    }

    free(clients);
    msgctl(serverQueue, IPC_RMID, NULL);

    printf("Server shut down\n");
}

int main()
{

    int key = ftok(PATH, 1);
    serverQueue = msgget(key, IPC_CREAT | 0666);
    printf("Server key: %d\n", key);

    signal(SIGINT, exitHandler);

    struct msgData *data = malloc(sizeof (msgData));
    SIZE = sizeof (msgData) - sizeof (long);
    newId = FIRST_ID;
    clients = malloc(MAXCLIENTS * sizeof(struct client));

    printf("Server started\n");

    while(1)
    {
        msgrcv(serverQueue, data, SIZE, -6, 0);

        switch(data->mtype){
            case STOP:
                printf("Handling STOP\n");
                handleStop(clients, data);
                break;
            case INIT:
                printf("Handling INIT\n");
                handleInit(clients, data);
                break;
            case LIST:
                printf("Handling LIST\n");
                handleList(clients, newId);
                break;
            case ONE:
                printf("Handling ONE\n");
                handleOne(clients, newId, data);
                break;
            case ALL:
                printf("Handling ALL\n");
                handleAll(clients, data);
                break;
        }
    }
}