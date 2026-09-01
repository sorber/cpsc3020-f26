#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv)
{

	if (fork() == 0)
  {
    setenv("JACOBSVAR","SOMETHINGELSE",1);
    execl("./envdemo", "./envdemo", NULL);

  }
  printf("Parent! JACOBSVAR=%s\n",getenv("JACOBSVAR"));

}
