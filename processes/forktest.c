

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int gx = 4;

int main()
{
    printf("%d: before\n", getpid());
    pid_t p = fork();
    if (p == 0)
    {
        gx = 3;
        printf("I'm a child! gx = %d\n", gx);
        sleep(2);
    }
    else
    {
        wait(NULL);
        // kill(p, SIGKILL);
        printf("I'm the parent. gx = %d\n", gx);
    }
    printf("%d: after %d\n", getpid(), p);
}