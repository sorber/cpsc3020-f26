#include "common.h"
#include <time.h>

int main(int argc, char **argv)
{
    int listenfd;
    struct sockaddr_in6 servaddr, from_address;
    char buff[MAXLINE + 1];
    char recvline[MAXLINE + 1];
    time_t ticks;

    ssize_t recv_bytes;
    socklen_t from_address_length = sizeof(from_address);
    int recv_count = 0;

    if ((listenfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        err_n_die("socket error.");

    // This next part is important if you want to accept BOTH ipv4 and ipv6 connections
    // Allow both IPv4 and IPv6 (Turn OFF IPv6-only mode)
    int no = 0;
    if (setsockopt(listenfd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0)
    {
        perror("Failed to set dual-stack option");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // Allow reusing the port
    // Without this, you might have to wait a second before you can restart the server
    int reuse = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("couldn't set SO_REUSEADDR");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(SERVER_PORT); /* server port */

    if ((bind(listenfd, (SA *)&servaddr, sizeof(servaddr))) < 0)
        err_n_die("bind error.");

    printf("Listening for datagrams on port %hu\n", SERVER_PORT);
    for (;;)
    {
        // this call blocks until a packet arrives
        recv_bytes = recvfrom(listenfd, recvline, MAXLINE, 0,
                              (SA *)&from_address, &from_address_length);
        recv_count++;
        recvline[recv_bytes] = 0; // null terminate the string

        if (recv_bytes < 0)
            err_n_die("receive error");

        if (recv_bytes != 51)
        {
            printf("Got packet: wrong length\n");
            continue;
        }
        // write the message to standard out.
        printf("%d:\t%s\t%s\t%d\n", recv_count, recvline, recvline + 25, recvline[50]);

        // now send a response.
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "Thanks. Recved at (%.24s)\r\n", ctime(&ticks));

        // note: normally, you may want to check the results from sendto and close
        // in case errors occur. For now, I'm just exiting.
        if ((sendto(listenfd, buff, strlen(buff), 0, (SA *)&from_address, from_address_length)) < 0)
            err_n_die("sendto error");
    }
}
