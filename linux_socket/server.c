#include <pthread.h>

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define TIMEOUT -1 // none //2 * 60 // 2 minutes
#define PORT 433

struct Sock
{
    int id;
    int latest; // latest update from client

    pthread_t service, timeouter; // threads
};

static void *service(void *arg);

static void *timeouter(void *arg);

int main()
{
    // Open Socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        printf("failed to open socket\n");
        return 1;
    }

    // Start Server
    struct sockaddr_in serv_addr = {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("failed to bind port\n");
        return 1;
    }

    listen(sockfd, 5);

    // Accepter Loop
    while (1)
    {
        // Accept Connection
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int new_socket = accept(sockfd, (struct sockaddr *)&client, &len);

        if (new_socket < 0)
        {
            printf("failed to accept connection\n");
            continue;
        }

        printf("new connection\n");

        // if (sock->id == -1) {
        // Launch threads
        struct Sock *sock = malloc(sizeof(struct Sock)); // TODO fix memory leak
        sock->id = new_socket;
        sock->latest = time(0);
        pthread_create(&sock->service, NULL, service, sock);
        if (TIMEOUT > 0)
            pthread_create(&sock->timeouter, NULL, timeouter, sock);

        // }
    }

    pthread_cancel(accept);
    close(sockfd);
    return 0;
}

static void *service(void *arg)
{
    struct Sock *sock = arg;

    int len;
    char buffer[256];

    // printf("started service\n");

    while (sock->id >= 0)
    {
        bzero(buffer, 256);
        if ((len = read(sock->id, buffer, sizeof(buffer))) < 0)
            continue;

        sock->latest = time(0);

        len--;
        buffer[len] = 0;

        // handle exit
        if (!strncasecmp(buffer, "exit", 4))
        {
            int id = sock->id;
            sock->id = -1;
            close(id);
            // free(sock);

            break;
        }

        printf("new message: %.*s", len, buffer);

        sleep(1);
    }

    // printf("connection closed\n");

    return NULL;
}

static void *timeouter(void *arg)
{
    struct Sock *sock = arg;

    do
    {
        sleep(1);

        if (sock->latest + TIMEOUT < time(0))
        {
            fprintf(stderr, "timeouted connection\n");
            int id = sock->id;
            sock->id = -1;
            close(id);
            // free(sock);
        }
    } while (sock->id >= 0);

    return NULL;
}