#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <hostname>\n", argv[0]);
        return EXIT_SUCCESS;
    }

    char *hostname = argv[1];
    struct addrinfo hints, *pfirstResult;

    memset(&hints, 0, sizeof(hints));
    // hints.ai_socktype = SOCK_STREAM; // I'm only interested in STREAM sockets.
    // hints.ai_protocol = IPPROTO_TCP;

    int error = getaddrinfo(hostname, NULL, &hints, &pfirstResult);

    if (error)
    {
        errx(1, "%s", gai_strerror(error));
    }
    struct addrinfo *curaddr;
    for (curaddr = pfirstResult; curaddr; curaddr = curaddr->ai_next)
    {
        printf("candidate:\n");
        printf("\tfamily:\t%d\n", curaddr->ai_family);
        printf("\tsocktype:\t%d\n", curaddr->ai_socktype);
        printf("\tprotocol:\t%d\n", curaddr->ai_protocol);

        printf("\taddress:\t");

        char addr_str[INET6_ADDRSTRLEN];
        void *addr_ptr;

        if (curaddr->ai_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)curaddr->ai_addr;
            addr_ptr = &(ipv4->sin_addr);
        }
        else if (curaddr->ai_family == AF_INET6)
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)curaddr->ai_addr;
            addr_ptr = &(ipv6->sin6_addr);
        }
        inet_ntop(curaddr->ai_family, addr_ptr, addr_str, sizeof(addr_str));

        printf("%s\n\n", addr_str);
    }

    freeaddrinfo(pfirstResult);
}