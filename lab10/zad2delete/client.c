#include "common.h"

int socket_fd;
char *ip = NULL;
char *port = NULL;
char *path = NULL;
char *client_unixpath = NULL;
int is_local = 0;

int trim(char *str)
{
    int i = strlen(str) - 1;
    while (i >= 0 && isspace(str[i]))
    {
        str[i] = '\0';
        i--;
    }

    i = 0;
    while (i < strlen(str) && isspace(str[i]))
    {
        i++;
    }

    // Modify string
    int j = 0;
    while (i < strlen(str))
    {
        str[j] = str[i];
        i++;
        j++;
    }

    str[j] = '\0';

    return i;
}

void handle_response(char *response)
{
    if (strncmp(response, "OK", 2) == 0)
    {
        printf(stdout, "Response: %s\n", response + 2);
    }
    else if (strncmp(response, "2ALL", 4) == 0 || strncmp(response, "2ONE", 4) == 0 || strncmp(response, "LIST", 4) == 0)
    {
        printf(stdout, "%s\n", response + 4);
    }
    else if (strncmp(response, "PING", 4) == 0)
    {
        if (write(socket_fd, "PONG", 4) == -1)
        {
            perror("write");
        }
    }
    else if (strncmp(response, "STOP", 4) == 0)
    {
        printf("Response: %s\n", response + 4);
        shutdown(socket_fd, SHUT_RDWR);
        close(socket_fd);

        if (is_local)
        {
            unlink(client_unixpath);

            free(path);
            free(client_unixpath);
        }
        else
        {
            free(ip);
            free(port);
        }

        kill(getppid(), SIGINT);
        exit(0);
    }
    else if (strncmp(response, "ERROR", 5) == 0)
    {
        printf("Response: %s\n", response + 5);
    }
    else
    {
        printf("Response: Unknown response\n");
    }
}

void exit_handler()
{
    if (send(socket_fd, "STOP", 4, 0) == -1)
    {
        perror("send");
        exit(1);
    }

    if (close(socket_fd) == -1)
    {
        perror("close");
        exit(1);
    }

    if (is_local)
    {
        unlink(client_unixpath);

        free(path);
        free(client_unixpath);
    }
    else
    {
        free(ip);
        free(port);
    }

    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s <name> <method> <ip:port|localname>\n", argv[0]);
        exit(1);
    }

    // Check what method is used to connect
    if (strcmp(argv[2], "local") == 0)
        is_local = 1;
    else if (strcmp(argv[2], "network") != 0)
    {
        printf("Method must be either 'local' or 'network'\n");
        exit(1);
    }

    // Parse ip and port or unixpath
    if (is_local)
    {
        path = malloc(strlen(argv[3]) + 1);
        strcpy(path, argv[3]);

        if (strlen(path) > UNIX_PATH_MAX)
        {
            printf("Unixpath too long\n");
            exit(1);
        }

        // Concatenate unixpath with client name
        client_unixpath = malloc(strlen(argv[1]) + strlen(argv[3]) + 1);
        strcpy(client_unixpath, argv[3]);
        strcat(client_unixpath, argv[1]);

        if (strlen(client_unixpath) > UNIX_PATH_MAX)
        {
            printf("Client name too long\n");
            exit(1);
        }
    }
    else
    {
        char *colon = strchr(argv[3], ':');
        if (colon == NULL)
        {
            printf("Invalid ip:port\n");
            exit(1);
        }

        ip = malloc(colon - argv[3] + 1);
        strncpy(ip, argv[3], colon - argv[3]);
        ip[colon - argv[3]] = '\0';

        port = malloc(strlen(colon + 1) + 1);
        strcpy(port, colon + 1);
    }

    // Create socket
    if (is_local)
        socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    else
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd == -1)
    {
        perror("socket");
        exit(1);
    }

    // Connect to server
    if (is_local)
    {
        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, path);

        struct sockaddr_un client_addr;
        client_addr.sun_family = AF_UNIX;
        strcpy(client_addr.sun_path, client_unixpath);

        // Bind to local socket
        if (bind(socket_fd, (const struct sockaddr *)&client_addr, sizeof(client_addr)) == -1)
        {
            perror("bind");
            exit(1);
        }

        if (connect(socket_fd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
        {
            perror("connect");
            exit(1);
        }
    }
    else
    {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(port));
        addr.sin_addr.s_addr = inet_addr(ip);

        if (connect(socket_fd, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
        {
            perror("connect");
            exit(1);
        }
    }

    signal(SIGINT, exit_handler);

    char buffer[LINE_MAX];

    strcpy(buffer, "INIT");
    strcat(buffer, argv[1]);
    send(socket_fd, buffer, strlen(buffer) + 1, 0);

    // Start listening for responses
    if (fork() == 0)
    {
        while (1)
        {
            if (recv(socket_fd, buffer, LINE_MAX, MSG_DONTWAIT) == -1)
            {
                // perror("recv");
                continue;
            }

            handle_response(buffer);
        }

        return 0;
    }

    // Read commands
    char command[LINE_MAX];
    while (fgets(command, LINE_MAX, stdin) != NULL)
    {
        // Remove newline
        command[strlen(command) - 1] = '\0';

        // Check if command is valid
        if (strlen(command) == 0)
            continue;

        if (strncmp(command, "STOP", 4) == 0)
        {
            exit_handler();
            break;
        }

        // Get command and message separated by space
        char *type = strtok(command, " ");
        char *message = strtok(NULL, "\n");

        trim(type);

        if (message == NULL)
            message = "";
        else
            trim(message);

        if (strcmp(type, "2ONE") == 0)
        {
            char *name = strtok(message, " ");
            char *msg = strtok(NULL, "\n");
            if (msg == NULL)
                msg = "";
            else
                trim(msg);
            sprintf(message, "%s %s", name, msg);
        }

        // Create string concatenated from type and message
        sprintf(command, "%s%s", type, message);

        // Send command
        if (send(socket_fd, command, strlen(command) + 1, 0) == -1)
        {
            printf("Error while sending command\n");
            exit(1);
        }
    }

    // Close socket
    if (close(socket_fd) == -1)
    {
        printf("Error while closing socket\n");
        exit(1);
    }

    // Free memory
    if (is_local)
    {
        // Remove client socket
        unlink(client_unixpath);

        free(path);
        free(client_unixpath);
    }
    else
    {
        free(ip);
        free(port);
    }
    return 0;
}
