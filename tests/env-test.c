#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv, char **envp)
{

	for (char **env = envp; *env != NULL; env++)
	{
		char *thisEnv = *env;

		printf("%s\n", thisEnv);
	}

	return 0;
}