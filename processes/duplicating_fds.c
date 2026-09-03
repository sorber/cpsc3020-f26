#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    // int file_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    FILE *fp = fopen("output.txt", "w");
    int file_fd = fileno(fp);

    if (file_fd < 0)
    {
        perror("Failed to open file");
        return 1;
    }

    //"connect STDOUT to file_fd"
    if (dup2(file_fd, STDOUT_FILENO) < 0)
    {
        perror("Failed to redirect stdout");
        return 1;
    }

    printf("This text goes directly into the file.\n");

    fclose(fp);
    // close(file_fd);
    return 0;
}
