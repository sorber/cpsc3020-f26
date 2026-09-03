#include "common.h"
#include <time.h>
#include <strings.h>

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    struct sockaddr_in servaddr;
    char buff[MAXLINE + 1];
    char recvline[MAXLINE + 1];
    time_t ticks;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_n_die("socket error.");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT); /* server port */

    if ((bind(listenfd, (SA *)&servaddr, sizeof(servaddr))) < 0)
        err_n_die("bind error.");

    if ((listen(listenfd, LISTENQ)) < 0)
        err_n_die("listen error.");

    for (;;)
    {
        // accept blocks until an incoming connection arrives
        // it returns a "file descriptor" to the connection
        printf("waiting for a connection...");
        fflush(stdout);
        connfd = accept(listenfd, (SA *)NULL, NULL);

        printf("got one! \n");
        if (connfd < 0)
        {
            if (errno == EPROTO || errno == ECONNABORTED)
            {
                continue; // not a fatal error. skip this round of the for loop
            }
            else
            {
                err_n_die("fatal error.");
            }
        }

        // Now read the client's message.
        while ((n = read(connfd, recvline, MAXLINE)) > 0)
        {
            recvline[n] = 0; /* null terminate */
            if (fputs(recvline, stdout) == EOF)
                err_n_die("fputs error");

            // detect the end of the message.
            if (recvline[n - 1] == '\n')
            {
                break;
            }
        }
        if (n < 0)
            err_n_die("read error");

        // now send a response.
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "Thanks. Recved at (%.24s)\r\n", ctime(&ticks));

        // note: normally, you may want to check the results from write and close
        // in case errors occur. This is a demo. So, for now, I'm ignoring them.
        write(connfd, buff, strlen(buff));
        close(connfd);
    }
}
