#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int clientSocket;
int totalArgs;
char *ip = NULL;
char *port = NULL;
char *unixpath = NULL;
char *name = NULL;
char* clientpath = NULL;

struct sockaddr_in sockaddr_storage;
socklen_t serverAddressLength;

void exit_handler()
{
    char * command = "STOP ";

    sendto(clientSocket, command, strlen(command), 0, (struct sockaddr *)&sockaddr_storage, serverAddressLength);

    kill(getppid(), SIGINT);

    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);

    free(name);
    if(totalArgs == 3)
    {
        unlink(unixpath);
        free(unixpath);
        free(clientpath);
    }
    else
    {
        free(ip);
        free(port);
    }

    printf("CLIENT: Shut down\n");
    exit(0);
}


int main(int argc, char **argv) {
    serverAddressLength = sizeof(sockaddr_storage);
    char buffer[BUFFER_SIZE];
    totalArgs = argc;
    if(argc == 3)
    {
        name = malloc(strlen(argv[1]) + 1);
        strcpy(name, argv[1]);
        unixpath = malloc(strlen(argv[2]) + 1);
        strcpy(unixpath, argv[2]);

        clientpath = malloc(strlen(argv[1]) + strlen(argv[3]) + 1);
        strcpy(clientpath, argv[3]);
        strcat(clientpath, argv[1]);

        clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);

        if (clientSocket == -1)
        {
            printf("Error while creating socket\n");
            exit(1);
        }

        struct sockaddr_un unixAddress;
        unixAddress.sun_family = AF_UNIX;
        strncpy(unixAddress.sun_path, clientpath, sizeof(unixAddress.sun_path) - 1);
        serverAddressLength = sizeof(struct sockaddr_un);

        memcpy(&sockaddr_storage, &unixAddress, serverAddressLength);

        if (bind(clientSocket, (const struct sockaddr *)&unixAddress, sizeof(unixAddress)) == -1)
        {
            perror("bind error");
            exit(1);
        }

    }
    else if(argc == 4)
    {
        name = malloc(strlen(argv[1]) + 1);
        strcpy(name, argv[1]);
        port = malloc(strlen(argv[2]) + 1);
        strcpy(port, argv[2]);
        ip = malloc(strlen(argv[3]) + 1);
        strcpy(ip, argv[3]);


        struct sockaddr_in serverAddress;
        // Tworzenie gniazda klienta
        clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientSocket == -1) {
            perror("Błąd podczas tworzenia gniazda klienta");
            exit(EXIT_FAILURE);
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr(ip);
        serverAddress.sin_port = htons(atoi(port));
        serverAddressLength = sizeof(struct sockaddr_in);
        memcpy(&sockaddr_storage, &serverAddress, serverAddressLength);
    }
    else
    {
        printf("Usage: %s <name> <localname>\n", argv[0]);
        printf("Usage: %s <name> <port> <ip>\n", argv[0]);
        exit(1);
    }

    snprintf(buffer, sizeof(buffer), "INIT;%s", name);

    sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&sockaddr_storage, serverAddressLength);
    
    signal(SIGINT, exit_handler);

    pid_t pid = fork();
    if (pid == -1) 
    {
        perror("Błąd podczas wywoływania fork");
        exit(EXIT_FAILURE);

    } 
    else if (pid == 0) 
    {
        // Proces potomny - odbieranie danych
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&sockaddr_storage, &serverAddressLength);
            if (bytesRead == -1) {
                perror("Błąd podczas odbierania danych");
                continue;
            }
            printf("Otrzymano wiadomość: %s\n", buffer);

            if (strncmp(buffer, "STOP", 4) == 0)
             {
                printf("SERVER: Shut down\n");
                kill(getppid(), SIGINT);
                shutdown(clientSocket, SHUT_RDWR);
                close(clientSocket);
                printf("CLIENT: Shut down\n");
                exit(0);
            }
        }
    } 
    else
    {
        // Proces rodzicielski - wysyłanie danych
        while (1) {
            printf("Podaj wiadomość: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';  // Usunięcie znaku nowej linii

            if (strncmp(buffer, "STOP", 4) == 0) 
            {
                exit_handler();
            }

            ssize_t bytesSent = sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&sockaddr_storage, serverAddressLength);
            if (bytesSent == -1) {
                perror("Błąd podczas wysyłania danych");
                continue;
            }
        }
    }

    return 0;
}