#include <stdlib.h>
#include <stdio.h>

int
chartoint(int num, char *cadena[], int * arrayarellenar)
{
	int i = 0;
	int base = 10;
	char *endptr;
	int errno = 0;

	for(; i < num; i++) {
		long val = strtol(cadena[i], &endptr, base);
		if (errno != 0 || *endptr != '\0') {
			return -1;
		}
		arrayarellenar[i] = val;
	}

	return 0;
}

void
printinverse(int num, int * array)
{
	int i = num - 1;

	for(; i >= 0; i--) {
		printf("%d\n", array[i]);
	}
}

int
main(int argc, char *argv[])
{
	int status = 0;
	argc--;
	argv++;

	if (argc < 1) {
		exit(EXIT_FAILURE);
	}

	int * array = malloc(sizeof(int) * argc);
	status = chartoint(argc, argv, array);
	if (status != 0) {
		free(array);
		exit(EXIT_FAILURE);
	}

	printinverse(argc, array);

	free(array);
	exit(EXIT_SUCCESS);
}