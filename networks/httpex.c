#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFFER_SIZE 4096

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3)
    {
        printf("usage: %s <hostname> [<auth>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *hostname = argv[1];
    const char *port = "80";
    const char *auth = NULL;
    if (argc == 3)
    {
        auth = argv[2];
    }

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Use TCP stream sockets

    int status = getaddrinfo(hostname, port, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }

    int sockfd = -1;
    struct addrinfo *p;
    for (p = res; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
            continue;

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0)
        {
            break;
        }

        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(res);

    if (sockfd == -1)
    {
        fprintf(stderr, "Failed to connect to %s\n", hostname);
        return 1;
    }

    char authheader[BUFFER_SIZE] = {0};
    if (auth)
    {
        sprintf(authheader, "Authorization: Basic %s\r\n", auth);
    }

    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request),
             "GET / HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "Content-Language: en-US\r\n"
             "Content-Type: text/html\r\n"
             "%s"
             "\r\n",
             hostname, authheader);

    printf("REQUEST:\n%s\nRESPONSE:\n", request);

    if (send(sockfd, request, strlen(request), 0) < 0)
    {
        perror("send");
        close(sockfd);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytes_received] = '\0'; // Null-terminate the received chunk
        printf("%s", buffer);
    }

    if (bytes_received < 0)
    {
        perror("recv");
    }

    close(sockfd);
    return 0;
}
