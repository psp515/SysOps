#ifndef LAB6_HELPER_H
#define LAB6_HELPER_H

#define MAXCLIENTS 20
#define MSGMAXLEN 1024
#define MAXDATE 20
#define FIRST_ID 0
#define SERVER_NAME "/SERV"

typedef enum{
    STOP = 1,
    LIST = 2,
    ALL = 3,
    ONE = 4,
    INIT = 5,
    SERVER_DOWN = 6,
} message;

struct msgData{
    long mtype;
    int client_id;
    int to_id;
    char buffer[MSGMAXLEN+1];
    char date[MAXDATE];
} msgData;

typedef struct client{
    int clientID;
    int queueKey;
    int status;
}client;

#endif //LAB6_HELPER_H
