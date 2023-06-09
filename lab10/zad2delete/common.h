#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>

#define MAX_BACKLOG 10
#define MAX_CLIENTS 10
#define PING_TIMEOUT 5

#ifndef MSG_CONFIRM
#define MSG_CONFIRM 0x800
#endif

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 104
#endif

socklen_t sockaddr_in_size = sizeof(struct sockaddr_in);
