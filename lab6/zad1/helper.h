#ifndef LAB6_HELPER_H
#define LAB6_HELPER_H

#define MAXCLIENTS 20
#define MSGMAXLEN 1024

typedef enum{
    STOP = 1,
    LIST = 2,
    ALL = 3,
    ONE = 4,
    INIT = 5,
    RESPONSE = 6,
    INFO = 7,
    SERVER_DOWN = 8
}message;

struct msgData{
    long mtype;
    char buffer[MSGMAXLEN+1];
} msgData;

typedef struct client{
    int clientID;
    int queueID;
    int status;
}client;

#endif //LAB6_HELPER_H
