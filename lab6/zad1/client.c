#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "helper.h"
#include "time.h"

#define MAXBUFFER 2048

long SIZE;
int key;
int client_id;
int queue;
int server_q;
time_t now;
struct tm *local_time;

void send2one(char *value, int id)
{
    char date_str[11];
    now = time(NULL);
    local_time = localtime(&now);
    strftime(date_str, 11, "%Y-%m-%d", local_time);

    struct msgData *message = malloc(sizeof (msgData));
    message->mtype = ONE;
    message->client_id = client_id;
    message->to_id = id;
    strcpy(message->date, date_str);
    strcpy(message->buffer, value);

    msgsnd(server_q, message, SIZE, 0);
}
void send2all(char *value)
{
    char date_str[11];
    now = time(NULL);
    local_time = localtime(&now);
    strftime(date_str, 11, "%Y-%m-%d", local_time);

    struct msgData *message = malloc(sizeof (msgData));
    message->mtype = ALL;
    message->client_id = client_id;
    strcpy(message->date, date_str);
    strcpy(message->buffer, value);

    msgsnd(server_q, message, SIZE, 0);
}
void sendList()
{
    char date_str[11];
    now = time(NULL);
    local_time = localtime(&now);
    strftime(date_str, 11, "%Y-%m-%d", local_time);

    struct msgData *message = malloc(sizeof (msgData));
    message->mtype = LIST;
    message->client_id = client_id;
    strcpy(message->date, date_str);

    msgsnd(server_q, message, SIZE, 0);
}
void sendStop()
{
    char date_str[11];
    now = time(NULL);
    local_time = localtime(&now);
    strftime(date_str, 11, "%Y-%m-%d", local_time);

    struct msgData *message = malloc(sizeof (msgData));
    message->mtype = STOP;
    message->client_id = client_id;
    strcpy(message->date, date_str);

    msgsnd(server_q, message, SIZE, 0);
    msgctl(queue, IPC_RMID, NULL);

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

int main()
{
    SIZE = sizeof (msgData) - sizeof (long);

    int server_k = ftok(PATH, 1);
    server_q = msgget(server_k, IPC_PRIVATE);

    key = ftok(PATH, (rand() % 1024) + 10);
    queue = msgget(key, IPC_CREAT | 0666);

    signal(SIGINT, sendStop);

    struct msgData *initData = malloc(sizeof (msgData));
    initData->mtype = INIT;
    initData->new_client_queue = key;
    msgsnd(server_q, initData,SIZE, 0);
    msgrcv(queue, initData, SIZE, 0, 0);

    if(initData->client_id == -1)
    {
        printf("Couldn't create client.\n");
        exit(1);
    }
    client_id = initData->client_id;
    printf("Connected To Server with %d\n", client_id);

    char command[16];
    char id[128];
    char message[MSGMAXLEN];
    char input[MAXBUFFER];

    while(1)
    {
        // from Server
        struct msgData * tmpMessage = malloc(sizeof (msgData));
        while (msgrcv(queue, tmpMessage, SIZE, 0, IPC_NOWAIT) >=0)
        {
            if(tmpMessage->mtype == STOP)
            {
                msgctl(queue, IPC_RMID, NULL);
                exit(0);
            }

            printf("From: %d, Data: %s, Date: %s", tmpMessage->client_id, tmpMessage->buffer, tmpMessage->date);
        }


        // to Server
        printf("Write command:");
        if(fgets(input, MAXBUFFER, stdin) != NULL)
        {
            int n = sscanf(input, "%s %s %[^\n]s", command, id, message);
            if (n == 1 && strcmp(command, "STOP") == 0)
            {
                printf("Received STOP command\n");
                sendStop();
            }
            else if (n == 1 && strcmp(command, "LIST") == 0)
            {
                printf("Received LIST command\n");
                sendList();
            }
            else if (n == 2 && strcmp(command, "2ALL") == 0)
            {
                printf("Received 2ALL command with message: %s\n", id);
                send2all(id);
            }
            else if (n == 3 && strcmp(command, "2ONE") == 0)
            {
                printf("Received 2ONE command with message: %s and client ID: %s\n", message, id);
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