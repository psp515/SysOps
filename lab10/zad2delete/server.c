#include "common.h"

// NOTE: for unix path use /tmp/<name_of_file>

struct pollfd fds[2];
char *port;
char *path;
char *client_names[TOTAL_CLIENTS];
int client_fds[TOTAL_CLIENTS];
socklen_t client_sizes[TOTAL_CLIENTS] = {0};
struct sockaddr_in *clients[TOTAL_CLIENTS];

pthread_t ping_thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

void kill_server(int signo)
{
    char *msg = "STOPServer has been stopped.";
    // Close all connections
    pthread_mutex_lock(&mutex);

    // Send to all connected clients
    for (int j = 0; j < TOTAL_CLIENTS; j++)
    {
        if (client_fds[j] == -1)
        {
            continue;
        }

        sendto(fds[client_fds[j]].fd, (const char *)msg, strlen(msg) + 1, 0, (const struct sockaddr *)clients[j], client_sizes[j]);

        // Client disconnected
        if (client_names[j] != NULL)
            free(client_names[j]);
        free(clients[j]);
    }

    shutdown(fds[0].fd, SHUT_RDWR);
    close(fds[0].fd);
    shutdown(fds[1].fd, SHUT_RDWR);
    close(fds[1].fd);

    // Remove local socket
    unlink(path);

    pthread_mutex_unlock(&mutex);
    printf(stdout, "\nServer is shutting down...\n");
    exit(0);
}

void *ping_clients()
{
    while (1)
    {
        sleep(PING_TIMEOUT);
        char *msg = "PING";

        pthread_mutex_lock(&mutex);
        // Send to all connected clients
        for (int j = 0; j < TOTAL_CLIENTS; j++)
        {
            if (client_fds[j] == -1)
            {
                continue;
            }

            if (sendto(fds[client_fds[j]].fd, (const char *)msg, strlen(msg) + 1, MSG_NOSIGNAL | MSG_CONFIRM, (const struct sockaddr *)clients[j], client_sizes[j]) == -1)
            {
                // Client disconnected
                clients[j]->sin_addr.s_addr = 0;
                if (client_names[j] != NULL)
                    free(client_names[j]);
                client_names[j] = NULL;
                client_fds[j] = -1;
                printf(stdout, "Client %d disconnected\n", j);
            }
        }

        pthread_mutex_unlock(&mutex);
    }
}

void handle_message(char *buffer, struct sockaddr_in *cliaddr, int fd_index, socklen_t cliaddr_size)
{
    // Get current date and time as string
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char time_str[20];
    sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900,
            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    char response[LINE_MAX];

    if (strncmp(buffer, "INIT", 4) == 0)
    {
        // Get client name
        char *client_name = buffer + 4;

        // Check if client is already registered
        int client_exists = 0;
        for (int j = 0; j < TOTAL_CLIENTS; j++)
        {
            if (client_fds[j] != -1 && strcmp(client_names[j], client_name) == 0)
            {
                client_exists = 1;
                break;
            }
        }

        if (client_exists)
        {
            strcpy(response, "STOPClient already registered.");
            sendto(fds[fd_index].fd, (const char *)response, strlen(response) + 1, 0, (const struct sockaddr *)cliaddr, cliaddr_size);
            return;
        }

        // Save client to clients array
        int j = 0;
        while (j < TOTAL_CLIENTS && clients[j]->sin_addr.s_addr != 0)
        {
            j++;
        }

        if (j == TOTAL_CLIENTS)
        {
            strcpy(response, "STOPServer is full. Please try again later!");
            sendto(fds[fd_index].fd, (const char *)response, strlen(response) + 1, 0, (const struct sockaddr *)cliaddr, cliaddr_size);
            return;
        }

        memcpy(clients[j], cliaddr, cliaddr_size);
        client_names[j] = calloc(strlen(buffer + 4) + 1, sizeof(char));
        strcpy(client_names[j], buffer + 4);
        client_fds[j] = fd_index;
        client_sizes[j] = cliaddr_size;

        strcpy(response, "OKClient connected successfully.");
        sendto(fds[fd_index].fd, (const char *)response, strlen(response) + 1, 0, (const struct sockaddr *)cliaddr, cliaddr_size);

        printf("Client %d: %s connected.\n", j, client_names[j]);

        // Save request to log file
        FILE *log_file = fopen("serverlog.txt", "a");
        printf(log_file, "%s Client: %s (id: %d) Request: %s\n", time_str, client_names[j], j, buffer);
        fclose(log_file);

        return;
    }

    // Find client in clients array
    int client_index = -1;

    for (int j = 0; j < TOTAL_CLIENTS; j++)
    {
        // Compare sockaddr_in structs using memcmp
        if (client_fds[j] != -1 && memcmp(clients[j], cliaddr, cliaddr_size) == 0)
        {
            client_index = j;
            break;
        }
    }

    if (client_index == -1)
    {
        strcpy(response, "STOPClient not registered!");
        sendto(fds[fd_index].fd, (const char *)response, strlen(response) + 1, 0, (const struct sockaddr *)cliaddr, cliaddr_size);
        return;
    }

    // Save request to log file
    FILE *log_file = fopen("serverlog.txt", "a");
    printf(log_file, "%s Client: %s (id: %d) Request: %s\n", time_str, client_names[client_index], client_index, buffer);
    fclose(log_file);

    printf("Client %d: %s sent message: %s\n", client_index, client_names[client_index], buffer);

    if (strncmp(buffer, "PONG", 4) == 0)
    {
        return;
    }
    else if (strncmp(buffer, "STOP", 4) == 0)
    {
        printf("Client %d: %s disconnected.\n", client_index, client_names[client_index]);

        // Remove client from clients array
        clients[client_index]->sin_addr.s_addr = 0;
        free(client_names[client_index]);
        client_names[client_index] = NULL;
        client_fds[client_index] = -1;

        strcpy(response, "STOPClient removed from server.");

        sendto(fds[fd_index].fd, (const char *)response, strlen(response) + 1, 0, (const struct sockaddr *)cliaddr, cliaddr_size);

        client_fds[client_index] = -1;
    }
    else if (strncmp(buffer, "LIST", 4) == 0)
    {
        // Print list of clients
        printf("List of clients:\n");
        for (int j = 0; j < TOTAL_CLIENTS; j++)
        {
            if (client_fds[j] != -1 && clients[j]->sin_addr.s_addr != 0)
            {
                printf("%s\n", client_names[j]);
            }
        }

        // // Send list of clients
        strcpy(response, "LISTClient list:");
        for (int j = 0; j < TOTAL_CLIENTS; j++)
        {
            if (client_fds[j] != -1 && clients[j]->sin_addr.s_addr != 0)
            {
                strcat(response, "\n");
                strcat(response, client_names[j]);
            }
        }

        sendto(fds[fd_index].fd, (const char *)response, strlen(response) + 1, 0, (const struct sockaddr *)cliaddr, client_sizes[client_index]);
    }
    else if (strncmp(buffer, "2ALL", 4) == 0)
    {
        // Prepare message
        char *message_to_send = calloc(LINE_MAX + 100, sizeof(char));
        char *message = strtok(buffer + 4, "");
        sprintf(message_to_send, "2ALLMessage from %s: %s\nTime: %s", client_names[client_index], message, time_str);

        for (int j = 0; j < TOTAL_CLIENTS; j++)
        {
            if (client_fds[j] != -1 && clients[j]->sin_addr.s_addr != 0 && j != client_index)
            {
                if (sendto(fds[client_fds[j]].fd, (const char *)message_to_send, strlen(message_to_send) + 1, 0, (const struct sockaddr *)clients[j], client_sizes[j]) == -1)
                {
                    perror("sendto");
                }
            }
        }

        free(message_to_send);
    }
    else if (strncmp(buffer, "2ONE", 4) == 0)
    {
        char *name = strtok(buffer + 4, " ");
        char *message = strtok(NULL, "");

        // Prepare message
        char *message_to_send = calloc(LINE_MAX + 100, sizeof(char));
        sprintf(message_to_send, "2ONEMessage from %s: %s\nTime: %s", client_names[client_index], message, time_str);

        for (int j = 0; j < TOTAL_CLIENTS; j++)
        {
            if (client_fds[j] != -1 && clients[j]->sin_addr.s_addr != 0 && strcmp(client_names[j], name) == 0)
            {
                sendto(fds[client_fds[j]].fd, (const char *)message_to_send, strlen(message_to_send) + 1, 0, (const struct sockaddr *)clients[j], client_sizes[j]);
                free(message_to_send);

                return;
            }
        }

        strcpy(response, "ERRORClient not found.");
        sendto(fds[fd_index].fd, (const char *)response, strlen(response) + 1, 0, (const struct sockaddr *)cliaddr, cliaddr_size);

        free(message_to_send);
    }
    else
    {
        strcpy(response, "ERRORUnknown command.");
        sendto(fds[fd_index].fd, (const char *)response, strlen(response) + 1, 0, (const struct sockaddr *)cliaddr, cliaddr_size);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf(stderr, "Usage: %s <port> <path>\n", argv[0]);
        exit(1);
    }

    port = argv[1];
    path = argv[2];

    signal(SIGINT, kill_server);

    int network_socket = init_network_socket(port);
    int local_socket = init_local_socket(path);

    fds[0].fd = network_socket;
    fds[0].events = POLLIN;
    fds[1].fd = local_socket;
    fds[1].events = POLLIN;

    struct sockaddr_in cliaddr;
    char buffer[LINE_MAX];

    for (int i = 0; i < TOTAL_CLIENTS; i++)
    {
        clients[i] = calloc(1, sizeof(struct sockaddr_in));
        client_fds[i] = -1;
        client_names[i] = NULL;
    }

    printf("Server listening on port %s...\n", port);

    while (1)
    {
        int res = poll(fds, 2, -1);
        if (res == -1)
        {
            perror("poll");
        }

        pthread_mutex_lock(&mutex);

        // Handle new connections
        for (int i = 0; i < 2; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                socklen_t cliaddr_size;
                int n = recvfrom(fds[i].fd, (char *)buffer, LINE_MAX, MSG_DONTWAIT, (struct sockaddr *)&cliaddr, &cliaddr_size);

                if (n == -1)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        continue;
                    }

                    perror("recvfrom");
                }

                handle_message(buffer, &cliaddr, i, cliaddr_size);

                fds[i].revents = 0;
            }
        }

        pthread_mutex_unlock(&mutex);
    }

    return 0;
}
