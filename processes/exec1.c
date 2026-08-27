// an example of how to use exec to run another program

#include <unistd.h>
#include <stdio.h>

// what happens here?
int main(int argc, char **argv)
{
    int x = 7;
    execl("/usr/bin/ls", "ls", "-a", "-l", NULL);
    perror("exec failed");
    x++;
    printf("%d\n", x);
}
