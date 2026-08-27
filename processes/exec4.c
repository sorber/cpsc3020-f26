// another example of how to use exec
// to run another program
// trying another variant.

#include <unistd.h>
#include <stdio.h>

// what happens here?
int main(int argc, char **argv, char **envp)
{
    char *args[4] = {"ls", "-al", ".", NULL};

    if (fork() == 0)
    {
        // need to include the path now...since you
        // dropped the "p"
        int i = execv("/usr/bin/ls", args);
        if (i < 0)
        {
            perror("execv");
        }
    }
}
