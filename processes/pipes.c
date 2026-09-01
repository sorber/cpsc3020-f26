#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

int main(void)
{
    int pipe_fd[2]; // two file descriptors
    pid_t pid;
    const char message[] = "Hello from the parent process!";
    char buffer[100];

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid = fork();

    if (pid == 0)
    {
        /* Child only reads from the pipe. */
        close(pipe_fd[PIPE_WRITE_END]);

        ssize_t bytes_read = read(pipe_fd[PIPE_READ_END], buffer, sizeof(buffer) - 1);
        if (bytes_read == -1)
        {
            perror("read");
            close(pipe_fd[PIPE_READ_END]);
            return EXIT_FAILURE;
        }

        buffer[bytes_read] = '\0';
        printf("Child received: %s\n", buffer);

        close(pipe_fd[PIPE_READ_END]);
        return EXIT_SUCCESS;
    }

    /* Parent only writes to the pipe. */
    close(pipe_fd[PIPE_READ_END]);

    if (write(pipe_fd[PIPE_WRITE_END], message, strlen(message)) == -1)
    {
        perror("write");
        close(pipe_fd[PIPE_WRITE_END]);
        wait(NULL);
        return EXIT_FAILURE;
    }

    printf("Parent sent: %s\n", message);
    close(pipe_fd[PIPE_WRITE_END]);
    wait(NULL);
    return EXIT_SUCCESS;
}