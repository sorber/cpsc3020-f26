//another example of how to use exec
//to run another program
//trying another variant.

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

//what happens here?
int main()
{
        int x = 7;
        char *args[4]={"ls","-al",".",NULL};

        if (fork() == 0)
        {
                int i = execvp("ls",args);
                if (i < 0)
                        perror("execvp");


        }
        wait(NULL);
        //this actually happens, this time.
        x++;
        printf("Child done x = %d\n",x);
}
