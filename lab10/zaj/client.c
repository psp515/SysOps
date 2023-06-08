#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>

#define PORT 7777

int main() {
    int fd = -1;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Error creating socket\n");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_zero[0] = '\0';

    if (connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) ==
        -1) {
        printf("Error connecting\n");
    }

    char buf[64];
    sprintf(buf, "My pid is %d", getpid());
    if (write(fd, buf, 64) == -1) {
        printf("Error sending message\n");
    }
    close(fd);

    return 0;
}


