#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

extern char **environ;

// int main(void)
// int main(int argc, char **argv)
int main(int argc, char **argv, char **envp)
{
	int index = 0;

	putenv("MYVAR2=PIZZA");
	setenv("JACOBSVAR", "WHATEVERVALUEIWANT", 0);

	// NOTE: if this loop used envp[index] instead of environ[index]
	// the new variables wouldn't show up

	// while (envp[index] != NULL)
	while (environ[index] != NULL)
	{
		printf("%s\n", environ[index]);
		index++;
	}

	char *pathvar = getenv("PATH");
	printf("my path is --- %s\n", pathvar);
	char *myvar = getenv("JACOBSVAR");
	printf("JACOBSVAR is --- %s\n", myvar);
}
