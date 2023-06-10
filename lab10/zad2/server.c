#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>


#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define UNIX_PATH_MAX 104

// uwaga operacje 2all i 2one wybmagaja uzycia id klienta ! 

//create mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    char name[20];
    struct sockaddr_in address;
} Client;

int serverSocket;
Client activeClients[MAX_CLIENTS];
struct pollfd fds[2];
char* path;

void kill_server(int signo)
{
   char stopMessage[] = "STOP";
    for (int i = 0; i < MAX_CLIENTS; i++) 
        sendto(serverSocket, stopMessage, sizeof(stopMessage), 0, (struct sockaddr *)&activeClients[i].address, sizeof(activeClients[i].address));
        
    shutdown(fds[0].fd, SHUT_RDWR);
    close(fds[0].fd);
    shutdown(fds[1].fd, SHUT_RDWR);
    close(fds[1].fd);

    exit(EXIT_SUCCESS);

    unlink(path);

    printf("\nServer is shutting down...\n");
    exit(0);
}

int init_network_socket(char *port)
{
    int network_socket;
    struct sockaddr_in server_address;

    network_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (network_socket == -1)
    {
        perror("socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(port));
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(network_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("bind");
        exit(1);
    }

    return network_socket;
}

int init_local_socket(char *path)
{
    if (strlen(path) > UNIX_PATH_MAX)
    {
        printf("Path too long\n");
        exit(1);
    }

    int local_socket;
    struct sockaddr_un server_address;

    local_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (local_socket == -1)
    {
        perror("socket");
        exit(1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, path);

    if (bind(local_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("bind");
        exit(1);
    }

    return local_socket;
}


void *pinger()
{
    while (1)
    {
        sleep(3);
        char *msg = "PING;";

        //send msg to all clients
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (activeClients[i].address.sin_addr.s_addr != 0)
            {
                pthread_mutex_lock(&mutex);
                if (sendto(serverSocket, msg, strlen(msg) + 1, MSG_NOSIGNAL | MSG_CONFIRM, (const struct sockaddr *)&activeClients[i].address, sizeof(activeClients[i].address)) == -1)
                {
                    activeClients[i].address.sin_addr.s_addr = 0;
                    printf("Client %d disconnected\n", i);
                }
                pthread_mutex_unlock(&mutex);
            }
        }
    }
}


int main(int argc, char *argv[]) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    char buffer[BUFFER_SIZE];
    int numClients = 0;

    char* port = argv[1];
    path = argv[2];

    signal(SIGINT, kill_server);

    int network_socket = init_network_socket(port);
    int local_socket = init_local_socket(path);

    fds[0].fd = network_socket;
    fds[0].events = POLLIN;
    fds[1].fd = local_socket;
    fds[1].events = POLLIN; 

    signal(SIGINT, kill_server);

    //pthread_t pinger_thread;
    //pthread_create(&pinger_thread, NULL, (void *(*)(void *))pinger, NULL);

    while (1) {

        int res = poll(fds, 2, -1);
        if (res == -1)
        {
            perror("poll");
        }   

        pthread_mutex_lock(&mutex);

        for (int i = 0; i < 2; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                // Odbieranie wiadomości od klienta
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recvfrom(fds[i].fd, buffer, sizeof(buffer), 0,
                                     (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (bytesRead == -1) {
            perror("Błąd podczas odbierania wiadomości");
            continue;
        }


            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            char time_str[20];
            sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

            FILE *log_file = fopen("log.txt", "a");
            fprintf(log_file, "TIME: %s DATA: %s\n", time_str, buffer);
            fclose(log_file);

        // Obsługa wiadomości od klienta
        if (strncmp(buffer, "LIST", 4) == 0) {
            // Zlecenie wypisania listy aktywnych klientów
            printf("Zlecenie LIST\n");

            // Wysyłanie listy aktywnych klientów do klienta
            char clientList[MAX_CLIENTS * 20];
            memset(clientList, 0, sizeof(clientList));
            memcpy(clientList, "LIST ", 5);
            for (int i = 0; i < numClients; i++) {
                strcat(clientList, activeClients[i].name);
                strcat(clientList, " ");
            }

            //rintf("Lista: %s\n", clientList);

            ssize_t bytesSent = sendto(fds[i].fd, clientList, strlen(clientList), 0, (struct sockaddr *)&clientAddress, clientAddressLength);

            if (bytesSent == -1) {
                perror("Błąd podczas wysyłania listy aktywnych klientów");
                continue;
            }
        } 
        else if (strncmp(buffer, "2ALL", 4) == 0) {
            // Zlecenie wysłania komunikatu do wszystkich klientów
            printf("Zlecenie 2ALL\n");

            // Przesyłanie komunikatu do wszystkich klientów
            for (int i = 0; i < numClients; i++) {
                if (memcmp(&clientAddress, &(activeClients[i].address), sizeof(clientAddress)) != 0) {
                    ssize_t bytesSent = sendto(fds[i].fd, buffer , strlen(buffer), 0,
                                               (struct sockaddr *)&(activeClients[i].address),
                                               sizeof(activeClients[i].address));
                    if (bytesSent == -1) {
                        perror("Błąd podczas wysyłania komunikatu do klienta");
                        continue;
                    }
                }
            }
        } else if (strncmp(buffer, "2ONE", 4) == 0) {
            // Zlecenie wysłania komunikatu do konkretnego klienta
            printf("Zlecenie 2ONE\n");
 
            // Wyszukiwanie klienta o podanym identyfikatorze
            char *spacePos = strchr(buffer + 5, ' ');
            if (spacePos != NULL) {
                int clientId = atoi(buffer + 5);
                for (int i = 0; i < numClients; i++) {
                    if (memcmp(&clientAddress, &(activeClients[i].address), sizeof(clientAddress)) != 0) {
                        if (clientId == i) {
                            ssize_t bytesSent = sendto(fds[i].fd, spacePos + 1, strlen(spacePos + 1), 0,
                                                       (struct sockaddr *)&(activeClients[i].address),
                                                       sizeof(activeClients[i].address));
                            if (bytesSent == -1) {
                                perror("Błąd podczas wysyłania komunikatu do klienta");
                                continue;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(buffer, "STOP", 4) == 0) {
            // Zgłoszenie zakończenia pracy klienta
            printf("Zgłoszenie STOP\n");

            // Usuwanie klienta z listy aktywnych klientów
            for (int i = 0; i < numClients; i++) {
                if (memcmp(&clientAddress, &(activeClients[i].address), sizeof(clientAddress)) == 0) {
                    memmove(&(activeClients[i]), &(activeClients[i + 1]), sizeof(Client) * (numClients - i - 1));
                    numClients--;
                    break;
                }
            }
        } 
        else if (strncmp(buffer, "INIT", 4) == 0) {
            // Inicjalizacja klienta
            printf("Zlecenie INIT\n");

            // Sprawdzanie, czy nazwa klienta jest unikalna
            int uniqueName = 1;
            for (int i = 0; i < numClients; i++) {
                if (memcmp(buffer + 5, activeClients[i].name, strlen(buffer + 5)) == 0) {
                    uniqueName = 0;
                    break;
                }
            }

            // Tworzenie odpowiedzi dla klienta
            char response[10];
            memset(response, 0, sizeof(response));

            if (uniqueName && numClients < MAX_CLIENTS) {
                strcpy(activeClients[numClients].name, buffer + 5);
                memcpy(&(activeClients[numClients].address), &clientAddress, sizeof(clientAddress));
                numClients++;
                strcpy(response, "OK");
            } 
            else 
            {
                strcpy(response, "STOP");
            }

            // Wysyłanie odpowiedzi do klienta
            ssize_t bytesSent = sendto(fds[i].fd, response, strlen(response), 0, (struct sockaddr *)&clientAddress, clientAddressLength);
            if (bytesSent == -1) {
                perror("Błąd podczas wysyłania odpowiedzi");
                continue;
            }
        }
        else
        {
            printf("Nieznane polecenie od klienta: %s\n %s", inet_ntoa(clientAddress.sin_addr), buffer);
        }

                fds[i].revents = 0;
            }
        }

        pthread_mutex_unlock(&mutex);

        
    }

    return 0;
}