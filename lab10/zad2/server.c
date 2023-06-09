#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024


// uwaga operacje 2all i 2one wybmagaja uzycia id klienta ! 


typedef struct {
    char name[20];
    struct sockaddr_in address;
} Client;

int serverSocket;
Client activeClients[MAX_CLIENTS];

void kill_server(int signo)
{
   char stopMessage[] = "STOP";
    for (int i = 0; i < MAX_CLIENTS; i++) 
        sendto(serverSocket, stopMessage, sizeof(stopMessage), 0, (struct sockaddr *)&activeClients[i].address, sizeof(activeClients[i].address));
        
    close(serverSocket);

    exit(EXIT_SUCCESS);

    printf("\nServer is shutting down...\n");
    exit(0);
}



int main() {
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    char buffer[BUFFER_SIZE];
    int numClients = 0;

    // Tworzenie gniazda serwera
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == -1) {
        perror("Błąd podczas tworzenia gniazda serwera");
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja adresu serwera
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);

    // Powiązanie gniazda serwera z adresem
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Błąd podczas powiązywania gniazda serwera z adresem");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, kill_server);

    while (1) {
        // Odbieranie wiadomości od klienta
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
                                     (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (bytesRead == -1) {
            perror("Błąd podczas odbierania wiadomości");
            continue;
        }

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

            ssize_t bytesSent = sendto(serverSocket, clientList, strlen(clientList), 0, (struct sockaddr *)&clientAddress, clientAddressLength);

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
                    ssize_t bytesSent = sendto(serverSocket, buffer , strlen(buffer), 0,
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
                            ssize_t bytesSent = sendto(serverSocket, spacePos + 1, strlen(spacePos + 1), 0,
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
            ssize_t bytesSent = sendto(serverSocket, response, strlen(response), 0, (struct sockaddr *)&clientAddress, clientAddressLength);
            if (bytesSent == -1) {
                perror("Błąd podczas wysyłania odpowiedzi");
                continue;
            }
        }
        else
        {
            printf("Nieznane polecenie od klienta: %s\n %s", inet_ntoa(clientAddress.sin_addr), buffer);
        }
    }

    // Zamknięcie gniazda serwera
    close(serverSocket);

    return 0;
}