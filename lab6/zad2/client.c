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
#include "helper.h"
#include "time.h"
#include "sys/msg.h"

#define MAXBUFFER 2048

mqd_t serverQueue;
mqd_t queue;
int clientId;
long SIZE;
time_t now;
struct tm *local_time;

void send2one(char *value, int id)
{
    now = time(NULL);
    local_time = localtime(&now);

    struct msgData *message = malloc(sizeof (msgData));
    message->mtype = ONE;
    message->client_id = clientId;
    message->to_id = id;
    strftime(message->date, 19, "%Y-%m-%d", local_time);
    strcpy(message->buffer, value);

    mq_send(serverQueue,(char *)message,SIZE,0);
}
void send2all(char *value)
{
    now = time(NULL);
    local_time = localtime(&now);

    struct msgData *message = malloc(sizeof (msgData));
    message->mtype = ALL;
    message->client_id = clientId;
    strftime(message->date, 19, "%Y-%m-%d", local_time);
    strcpy(message->buffer, value);


    mq_send(serverQueue,(char *)message,SIZE,0);
}
void sendList()
{
    now = time(NULL);
    local_time = localtime(&now);

    struct msgData *message = malloc(sizeof (msgData));
    message->mtype = LIST;
    message->client_id = clientId;
    strftime(message->date, 19, "%Y-%m-%d", local_time);

    mq_send(serverQueue,(char *)message,SIZE,0);
}
void sendStop()
{
    now = time(NULL);
    local_time = localtime(&now);

    struct msgData *message = malloc(sizeof (msgData));
    message->mtype = STOP;
    message->client_id = clientId;
    strftime(message->date, 19, "%Y-%m-%d", local_time);

    mq_send(serverQueue,(char *)message,SIZE,0);

    exit(0);
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

void exitHandler()
{
    printf("Client shut down\n");
}

int is_empty(mqd_t q)
{
    struct mq_attr attr;
    mq_getattr(q, &attr);
    return attr.mq_curmsgs == 0;
}

int main()
{
    SIZE = sizeof (msgData);

    char name[5];
    printf("Pass 5 letter word starting from '/'\n");
    fgets(name, 5, stdin);

    if(name[0] !=   '/')
    {
        printf("Error invalid name\n");
        exit(1);
    }

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = SIZE;
    attr.mq_curmsgs = 0;

    queue = mq_open(name, O_CREAT | O_RDWR, 0666, &attr);
    serverQueue = mq_open(SERVER_NAME, O_RDWR);

    struct msgData *initData = malloc(SIZE);
    initData->mtype = INIT;
    strcpy(initData->buffer, name);

    mq_send(serverQueue,(char *) initData, SIZE, initData->mtype);
    printf("Send request\n");
    int x = mq_receive(queue,(char *) initData, SIZE, NULL);
    if(x == -1)
    {
        printf("Couldn't create client - bad response.\n");
        exit(1);
    }

    printf("Received response\n");

    if(initData->client_id == -1)
    {
        printf("Couldn't create client.\n");
        exit(1);
    }

    clientId = initData->client_id;
    printf("Connected To Server with %d\n", clientId);

    atexit(sendStop);
    signal(SIGINT, exitHandler);

    char command[16];
    char id[128];
    char message[MSGMAXLEN];
    char input[MAXBUFFER];

    struct msgData * tmpMessage = malloc(sizeof (msgData));

    while(1)
    {
        // from Server
        while(!is_empty(queue))
        {
            mq_receive(queue, (char *) tmpMessage, SIZE, NULL);

            if (tmpMessage->mtype == SERVER_DOWN) {
                mq_close(serverQueue);
                mq_close(queue);
                printf("Client closed\n");
                exit(0);
            }

            printf("From: %d, Data: %s, Date: %s\n", tmpMessage->client_id, tmpMessage->buffer, tmpMessage->date);
        }

        // to Server
        printf("Write command:");
        if(fgets(input, MAXBUFFER, stdin) != NULL)
        {
            int n = sscanf(input, "%s %s %[^\n]s", command, id, message);
            if (n == 1 && strcmp(command, "STOP") == 0)
            {
                printf( "Sending STOP command\n");
                sendStop();
            }
            else if (n == 1 && strcmp(command, "LIST") == 0)
            {
                printf("Sending LIST command\n");
                sendList();
            }
            else if (n == 2 && strcmp(command, "2ALL") == 0)
            {
                printf("Sending 2ALL command with message: %s\n", id);
                send2all(id);
            }
            else if (n == 3 && strcmp(command, "2ONE") == 0)
            {
                printf("Sending 2ONE command with message: %s and client ID: %s\n", message, id);
                send2one(message, parse(id));
            }
            else
            {
                printf("Invalid command\n");
                printf("%s\n", input);
            }
        }
    }
}