#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Expected one argument\n");
	}
	
	int ret = atoi(argv[1]);
	
	
	return ret;
}