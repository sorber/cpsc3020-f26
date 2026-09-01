#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

void print_fd_table()
{
    printf("\n--- FD Table ---\n");

    // Open the directory natively
    DIR *dir = opendir("/proc/self/fd");
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip "." and ".." directories
        if (entry->d_name[0] == '.')
            continue;

        // Skip the FD that this 'opendir' call just opened (usually 3 or 4)
        // This strips away the "observer effect"!
        if (atoi(entry->d_name) == dirfd(dir))
            continue;

        // Read where the link points
        char path[1024];
        char target[1024];
        snprintf(path, sizeof(path), "/proc/self/fd/%s", entry->d_name);
        ssize_t len = readlink(path, target, sizeof(target) - 1);
        if (len != -1)
        {
            target[len] = '\0';
            printf("  FD %s -> %s\n", entry->d_name, target);
        }
    }
    closedir(dir);
}

int main()
{
    print_fd_table(); // Shows default FDs: 0 (stdin), 1 (stdout), 2 (stderr), 3 (ls command)

    // open a regular file
    int fd_file = open("test.txt", O_CREAT | O_WRONLY, 0644);
    print_fd_table(); // Shows a new numeric entry pointing to test.txt

    // create a pipe
    int pipe_fds[2];
    pipe(pipe_fds);
    print_fd_table(); // Shows two new entries pointing to anonymous pipes

    // clean up
    close(fd_file);
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    unlink("test.txt"); // remove the file we created

    print_fd_table(); // All done.

    return 0;
}
