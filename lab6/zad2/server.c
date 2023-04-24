//
// Created by psp515 on 24/04/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "helper.h"
#include <fcntl.h>
#include <mqueue.h>


char clients[MAXCLIENTS][100];
int newId = FIRST_ID;
mqd_t serverQueue;
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

void handleAll(struct msgData* buff)
{
    for(int i =0; i < newId; i++)
    {
        if(strcmp(clients[i], "") != 0)
        {
            mqd_t tmp = mq_open(clients[i], O_RDWR);
            mq_send(tmp, (char *)buff,SIZE,0);
            mq_close(tmp);
        }
    }
}
void handleOne(struct msgData* buff)
{
    if(strcmp(clients[buff->to_id], "") != 0)
    {
        mqd_t tmp = mq_open(clients[buff->to_id], O_RDWR);
        mq_send(tmp, (char *)buff,SIZE,0);
        mq_close(tmp);
    }
}

void handleList()
{
    for(int i = 0; i < newId; i++)
        if(strcmp(clients[i], "") != 0)
            printf("id: %d, q_key: %s\n", i, clients[i]);
}
void handleInit(struct msgData* buff)
{
    mqd_t tmp = mq_open(buff->buffer, O_RDWR);
    struct msgData* response = malloc(SIZE);
    response->mtype = INIT;

    if(newId >= MAXCLIENTS)
    {
        response->client_id = -1;
        mq_send(tmp, (char *)response,SIZE,0);
        mq_close(tmp);
    }

    strcpy(clients[newId], buff->buffer);
    response->client_id = newId;
    printf("New client id: %d\n",response->client_id);
    mq_send(tmp, (char *)response,SIZE,0);
    mq_close(tmp);
    newId = newId + 1;
}
void handleStop(struct msgData* buff)
{
    int id = buff->client_id;

    if (id < 0 || id >= MAXCLIENTS)
        return;

    strcpy(clients[id], "");
}
void stopServer()
{
    struct msgData *data = malloc(SIZE);
    data->mtype = SERVER_DOWN;

    for(int i = 0; i < newId; i++)
        if(strcmp(clients[i], "") != 0)
        {
            mqd_t client = mq_open(clients[i], O_RDWR);
            mq_send(client, (char *)data, SIZE, 0);
            mq_receive(serverQueue, (char *)data, SIZE, NULL);
            mq_close(client);
        }

    mq_close(serverQueue);
    exit(0);
}
void exitHandler()
{
    printf("Server shut down\n");
}

int main()
{
    for(int i = 0; i < MAXCLIENTS; i++)
        strcpy(clients[i], "");

    SIZE = sizeof (msgData);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = SIZE;
    attr.mq_curmsgs = 0;

    serverQueue = mq_open(SERVER_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (serverQueue == -1) {
        perror("mq_open");
        exit(1);
    }

    atexit(stopServer);
    signal(SIGINT, exitHandler);

    printf("Server started\n");

    struct msgData *message = malloc(SIZE);

    while(1)
    {
        int status =  mq_receive(serverQueue, (char *)message, SIZE, 0);

        if(status == -1)
        {
            printf("Received error.");
            exit(-1);
        }

        switch(message->mtype){
            case STOP:
                printf("Handling STOP\n");
                handleStop(message);
                break;
            case INIT:
                printf("Handling INIT\n");
                handleInit(message);
                break;
            case LIST:
                printf("Handling LIST\n");
                handleList();
                break;
            case ONE:
                printf("Handling ONE\n");
                handleOne(message);
                break;
            case ALL:
                printf("Handling ALL\n");
                handleAll(message);
                break;
        }
    }
}