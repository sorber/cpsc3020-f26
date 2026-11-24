#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdbool.h>

int main(void)
{
    fd_set read_fds; // the set of file descriptors to read from
    struct timeval timeout;
    char buffer[256];

    // clear the set
    FD_ZERO(&read_fds);

    // stdin to the set to check.
    FD_SET(STDIN_FILENO, &read_fds);

    // one more fd
    FILE *fp = fopen("Makefile", "r");
    int fd = fileno(fp);
    FD_SET(fd, &read_fds);

    // use a 5-second timeout
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    printf("Waiting for something to be ready (5 seconds):\n");

    // nfds must be the highest-numbered file descriptor + 1
    int ready, maxfd = fd;
    bool filedone = false;
    bool stdindone = false;

    while ((ready = select(maxfd + 1, &read_fds, NULL, NULL, &timeout)) > 0)
    {
        // printf("...(%d)\n", ready);
        for (int i = 0; i < maxfd + 1; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                // file descriptor i is ready to read.
                ssize_t bytes_read = read(i, buffer, sizeof(buffer) - 1);
                if (bytes_read == 0)
                {
                    if (i != STDIN_FILENO)
                    {
                        filedone = true;
                        fclose(fp);
                    }
                    else
                    {
                        stdindone = true;
                    }
                }
                if (bytes_read > 0)
                {
                    buffer[bytes_read] = '\0';
                    printf("Got %ldb from fd #%d: %.*s...\n", bytes_read, i, 10, buffer);
                    fflush(stdout);
                }
            }
        }
        if (stdindone && filedone)
            break;

        // rebuild the set
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        maxfd = STDIN_FILENO + 1;
        if (!filedone)
        {
            FD_SET(fd, &read_fds);
            maxfd = fd > STDIN_FILENO ? fd + 1 : maxfd;
        }
        // reset the timeout
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
    }
    if (ready == -1)
    {
        perror("select");
        return EXIT_FAILURE;
    }
    else if (ready == 0)
    {
        printf("Timeout! No input received within 5 seconds.\n");
    }

    return EXIT_SUCCESS;
}
