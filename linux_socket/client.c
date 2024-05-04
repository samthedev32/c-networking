#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 433
#define ADDRESS "localhost"

int main()
{
    // Open Socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        printf("failed to open socket\n");
        return 1;
    }

    // Get server host
    struct hostent *server = gethostbyname(ADDRESS);
    if (!server)
    {
        printf("no such host\n");
        return 2;
    }

    // Connect to server
    struct sockaddr_in serv_addr;
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(PORT);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("failed to connect\n");
        return 3;
    }

    while (1)
    {
        char buf[256];
        scanf("%[^\n]s", buf);
        size_t len = strlen(buf);

        int n = write(sockfd, buf, len);
        if (n != len)
        {
            printf("failed to send message\n");
        }
    }

    close(sockfd);
    return 0;
}