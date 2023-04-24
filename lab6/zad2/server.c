//
// Created by psp515 on 24/04/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>


int clients[MAXCLIENTS];
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

}
void handleOne(struct msgData* buff)
{

}
void handleList()
{
    for(int i = 0; i < newId; i++)
        if(clients[i] != 0)
            printf("id: %d, q_key: %d\n", i, clients[i]);
}
void handleStop(struct msgData* buff)
{

}
void handleInit(struct msgData* buff)
{
}

void stopServer()
{
    struct msgData *data;
    data->mtype = SERVER_DOWN;

    for(int i = 0; i < newId; i++)
        if(clients[i] != -1)
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
    exit(0);
}

int main()
{
    for(int i =0; i < MAXCLIENTS;i++)
        clients[i] = -1;

    SIZE = sizeof (msgData);

    struct mq_attr attr
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == -1) {
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
            printf("Receive error.")
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