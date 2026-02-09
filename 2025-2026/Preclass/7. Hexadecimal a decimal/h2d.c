#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	argc--;
	argv++;

	if (argc != 1) {
		exit(EXIT_FAILURE);
	}

	char *endptr;
	long val;
	errno = 0;

	val = strtol(argv[0], &endptr, 10);
	if (errno != 0 || *endptr != '\0') {
		exit(EXIT_FAILURE);
	}

	printf("%lX\n", val);

	exit(EXIT_SUCCESS);
}