/*
This is an example UDP client, that transmits a message to a server (ipv6 version)
*/

/**
 common.h includes commonly needed header files and helper functions
        like err_n_die(), which is a variant of printf.
        It also defines some values like MAXLINE and SERVER_PORT.
**/
#include "common.h"
#include <strings.h>

int main(int argc, char **argv)
{
    int sockfd;
    int sendbytes;
    struct sockaddr_in6 servaddr, fromaddr;
    char sendline[MAXLINE + 1];
    char recvline[MAXLINE + 1];
    socklen_t servaddr_length, fromaddr_length;

    if (argc != 3)
        err_n_die("usage: %s <ipv6 server address> <name>", argv[0]);

    // setting up an address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(SERVER_PORT); /* udp server port */

    if (inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr) <= 0)
        err_n_die("inet_pton error for %s ", argv[1]);

    // I want a "socket", for an internet connection, using datagrams (UDP)
    if ((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        err_n_die("Error while creating the socket!");

    // We've got our socket. Let's send something!
    sendbytes = strlen(argv[2]);

    printf("Sending text \"%s\" to %s, port %d\n", argv[2], argv[1], SERVER_PORT);
    // send the message
    if (sendto(sockfd, argv[2], sendbytes, 0, (SA *)&servaddr, sizeof(servaddr)) < 0)
        err_n_die("sendto error");

    // Now read the server's response.
    if (recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL) < 0)
        err_n_die("receive error");

    fprintf(stdout, "%s\n", recvline);

    exit(EXIT_SUCCESS); // end successfully!
}
