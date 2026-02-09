#include <stdlib.h>
#include <stdio.h>

void
inverseprint(int num, char * cadena[])
{
	int i = num - 1;

	for(; i >= 0; i--) {
		printf("%s\n", cadena[i]);
	}
}

int
main(int argc, char * argv[])
{
	argc--;
	argv++;

	if(argc < 1) {
		exit(EXIT_FAILURE);
	}

	inverseprint(argc, argv);

	exit(EXIT_SUCCESS);
}