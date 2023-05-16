#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define UNIX_PATH_MAX 108
#define SOCK_PATH "/tmp/kol2_socket" // Changed this so that the program doesn't have to have elevated permissions.


int main() {
    int fd = -1;

    /*********************************************
    Utworz socket domeny unixowej typu datagramowego
    Utworz strukture adresowa ustawiajac adres/sciezke komunikacji na SOCK_PATH
    Zbinduj socket z adresem/sciezka SOCK_PATH
    **********************************************/


    struct sockaddr_un server_addr;
    // Zeroing out the address buffer.
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) != 0)
    {
        perror("Failed to bind socket");
        return 1;
    }
    /////////////////////////////////////////////////////



    char buf[20];
    if(read(fd, buf, 20) == -1)
        perror("Error receiving message");
    int val = atoi(buf);
    printf("%d square is: %d\n",val,val*val);

    
    /***************************
    Posprzataj po sockecie
    ****************************/

    //////////////////////////
    return 0;
}

