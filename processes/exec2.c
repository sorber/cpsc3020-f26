//another example of how to use exec
//to run another program

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>


//what happens here?
int main()
{
    int x = 7;
    pid_t p;

    if ((p=fork())==0)
    {
    	execlp("ls","ls","-al",NULL);
      return 0;
    }
    //this actually happens, this time.
    wait(NULL);
    x++;
    printf("x = %d\n",x);
}
