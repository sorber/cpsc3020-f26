#include "common.h"
#include <time.h>

int main(int argc, char **argv)
{
    int listenfd, n;
    struct sockaddr_in servaddr, from_address;
    char buff[MAXLINE + 1];
    char recvline[MAXLINE + 1];
    time_t ticks;

    ssize_t recv_bytes;
    socklen_t from_address_length = sizeof(from_address);
    int recv_count = 0;

    if ((listenfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        err_n_die("socket error.");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT); /* server port */

    if ((bind(listenfd, (SA *)&servaddr, sizeof(servaddr))) < 0)
        err_n_die("bind error.");

    // note that listen and accept are not needed (UDP doesn't use connections)

    for (;;)
    {
        // this call blocks until a packet arrives
        recv_bytes = recvfrom(listenfd, recvline, MAXLINE, 0,
                              (SA *)&from_address, &from_address_length);
        recv_count++;
        recvline[recv_bytes] = 0; // null terminate the string

        if (recv_bytes < 0)
            err_n_die("receive error");

        // write the message to standard out.
        printf("%s\n", recvline);

        // now send a response.
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "Thanks. Recved at (%.24s)\r\n", ctime(&ticks));

        // note: normally, you may want to check the results from sendto and close
        // in case errors occur. For now, I'm just exiting.
        if ((sendto(listenfd, buff, strlen(buff), 0, (SA *)&from_address, from_address_length)) < 0)
            err_n_die("sendto error");
    }
}
