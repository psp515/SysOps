#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>
#include "helper.h"

static int running = 1;

void exitHandler()
{
    printf("Server shut down\n");
    running = 0;
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

void handleList(client* clients, struct msgData buff)
{

}

void handleAll(client* clients, struct msgData buff)
{
    int id = parse(buff.buffer);

}

void handleOne(client* clients, struct msgData buff)
{
    int id = parse(buff.buffer);


    //TODO chECK
    clients[id].status = 0;
}

void handleStop(client* clients, struct msgData buff)
{
    int id = parse(buff.buffer);

    if(id > MAXCLIENTS && clients[id].status != -1)
        return;

    clients[id].status = 0;
}

int handleInit(int id, client* clients, struct msgData buff)
{
    int clientQueue = parse(buff.buffer);
    client newClient = {id, clientQueue,1};
    clients[id] = newClient;
    return id + 1;
}

int main(int n, char** args)
{
    if(n != 2)
    {
        printf("Invalid number of arguments");
        exit(1);
    }

    int key = parse(args[1]);
    int queueKey = msgget(key, IPC_PRIVATE);
    signal(SIGINT, exitHandler);

    struct msgData data;
    int newId = 0;

    client* clients =  malloc(MAXCLIENTS * sizeof(struct client));

    FILE* writeFile = fopen("tmp.txt","w");
    printf("Server started\n");

    while(running)
    {
        printf("Waiting for message\n");
        size_t nRead = msgrcv(queueKey, &msgData, sizeof(msgData) - sizeof(long), -100, MSG_NOERROR);

        printf("Received message\n");
        msgData.buffer[nRead] = 0;

        switch(msgData.mtype){
            case STOP:
                puts("Handling STOP");
                handleStop(clients, msgData, writeFile);
                break;
            case INIT:
                if(newId < 20)
                    newId = handleInit(newId, clients, msgData, writeFile);
                else
                {
                    printf("Cannot and new Client.\n");
                }

                break;

            case LIST:
                puts("Handling LIST");
                handleList(clients,msgData, writeFile);
                break;
        }
    }

    for(int i = 0; i < newId; i++)
    {
        if(clients[i].status)
        {
            msgData.mtype = SERVER_DOWN;
            msgsnd(clients[i].queueID, &msgData, MSGMAX, IPC_NOWAIT);
        }
    }

    free(clients);
    fclose(writeFile);
    msgctl(queueKey, IPC_RMID, NULL);

    printf("Server off\n");

    exit(0);
}