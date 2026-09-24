#include "common.h"
#include <time.h>
#include <strings.h>
#include <pthread.h>

void *connection_func(void *);

void handle_connection(int connectionfd)
{
    char recvline[MAXLINE + 1];
    char buff[MAXLINE + 1];
    time_t starttime, endtime;
    int bytesread;
    double elapsed;

    time(&starttime);

    // Read the client's message.
    while ((bytesread = read(connectionfd, recvline, MAXLINE)) > 0)
    {
        recvline[bytesread] = 0; /* null terminate */
        if (fputs(recvline, stdout) == EOF)
            err_n_die("fputs error");

        // detect the end of the message.
        if (recvline[bytesread - 1] == '\n')
        {
            break;
        }
    }
    if (bytesread < 0)
        err_n_die("read error");

    sleep(2);
    // now send a response.
    time(&endtime);
    elapsed = difftime(endtime, starttime);
    snprintf(buff, sizeof(buff), "Thanks. Recved at (%.24s) (response time = %lf)\r\n", ctime(&starttime), elapsed);

    write(connectionfd, buff, strlen(buff));
    close(connectionfd);
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_n_die("socket error.");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT); /* server port */

    int reuse = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

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

        pthread_t mythread;
        int *argptr = malloc(sizeof(int));
        *argptr = connfd;
        pthread_create(&mythread, NULL, connection_func, argptr);
        // handle_connection(connfd);
    }
}

void *connection_func(void *arg)
{
    int *argptr = arg;

    handle_connection(*argptr);
    free(arg);
    return NULL;
}
