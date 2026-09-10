// unix_socket_demo.c
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOCKET_PATH "/tmp/demo.sock"

void server(void)
{
    int fd, client_fd;
    struct sockaddr_un addr;
    char buffer[128];

    fd = socket(AF_UNIX, SOCK_STREAM, 0); // can also use SOCK_DGRAM

    unlink(SOCKET_PATH); // Remove old socket "file"

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(fd, 1);

    printf("Server waiting...\n");
    client_fd = accept(fd, NULL, NULL);

    read(client_fd, buffer, sizeof(buffer) - 1);
    printf("Server received: %s\n", buffer);

    write(client_fd, "Hello from server!", 19);

    close(client_fd);
    close(fd);
    unlink(SOCKET_PATH);
}

void client(void)
{
    int fd;
    struct sockaddr_un addr;
    char buffer[128] = {0};

    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    connect(fd, (struct sockaddr *)&addr, sizeof(addr));

    write(fd, "Hello from client!", 19);

    read(fd, buffer, sizeof(buffer) - 1);
    printf("Client received: %s\n", buffer);

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s server|client\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "server") == 0)
        server();
    else if (strcmp(argv[1], "client") == 0)
        client();
    else
        return 1;

    return 0;
}