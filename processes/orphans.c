#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t pid = fork();

    if (pid > 0)
    {
        // The parent exits without waiting for the child.
        printf("Parent: PID = %ld, child PID = %ld\n",
               (long)getpid(), (long)pid);
        printf("Parent: exiting now...\n");
        fflush(stdout);
        return EXIT_SUCCESS;
    }

    // Give the parent time to exit before checking our parent PID.
    printf("Child: PID = %ld, initial PPID = %ld\n", (long)getpid(), (long)getppid());
    fflush(stdout);

    sleep(2);

    printf("Child: after the parent exits, PPID = %ld\n", (long)getppid());
    printf("Child: I am an orphan, adopted by the init process.\n");

    return EXIT_SUCCESS;
}