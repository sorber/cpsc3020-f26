/*
This is an example concurrent TCP client, that transmits a single message to a TCP server.
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> /* for inet_pton, and the like */
#include <stdarg.h>    /* for variable argument functions, like err_n_die */
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <ctype.h>

#define BUFFER_SIZE 4096

void err_n_die(const char *fmt, ...)
{
    int errno_save;
    va_list ap;

    // all system calls can set errno, so we need to save it now
    errno_save = errno;

    // print out the fmt+args to standard out
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    // print out error message is errno was set.
    if (errno_save != 0)
    {
        fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }
    va_end(ap);

    // this is the ..and_die part. Exit with an error.
    exit(EXIT_FAILURE);
}

static int sendall(int fd, void *buffer, size_t length)
{
    unsigned char *temp_ptr = buffer;
    while (length > 0)
    {
        ssize_t n;
        n = send(fd, temp_ptr, length, 0);

        if (n < 0 && errno == EINTR)
            continue;

        if (n <= 0)
            err_n_die("send failed (n=%d)", n);

        temp_ptr += n;
        length -= (size_t)n;
    }
    return 1;
}

int main(int argc, char **argv)
{
    int sockfd, n;
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_n_die("usage: %s <server address>", argv[0]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_n_die("Error while creating the socket!");

    // bzero(&servaddr, sizeof(servaddr));
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // use IPv4
    servaddr.sin_port = htons(9899);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        err_n_die("inet_pton error for %s ", argv[1]);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        err_n_die("connect failed!");

    // We're connected. Send the request.
    // I'm sending it in pieces. You could send it all at once.
    uint8_t buffer[BUFFER_SIZE];
    char write_data[] = "/EXAMPLE.TXT\0This is a test file for demonstration purposes only!";
    uint8_t *tempptr = buffer;

    // magic
    ((uint32_t *)tempptr)[0] = htonl(0xDEADCA75u);
    tempptr += sizeof(uint32_t);
    *(tempptr++) = 1; // write
    // requestid
    *(tempptr++) = 0x00;
    *(tempptr++) = 0x01;
    // data length
    ((uint32_t *)tempptr)[0] = htonl(sizeof write_data);
    tempptr += 4;

    // send header
    sendall(sockfd, buffer, 11);
    sendall(sockfd, write_data, sizeof write_data);

    // Now read the server's response.
    printf("Waiting for server...\n");
    int bytes_received = 0;
    while ((n = recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        bytes_received += n;
        printf("RESPONSE: ");
        for (int i = 0; i < n; i++)
        {
            printf("%02X ", buffer[i]);
        }
        printf("\n");
        if (bytes_received >= 11)
        {
            close(sockfd);
            break;
        }
    }
    if (n < 0)
        err_n_die("read error");

    exit(EXIT_SUCCESS);
}
