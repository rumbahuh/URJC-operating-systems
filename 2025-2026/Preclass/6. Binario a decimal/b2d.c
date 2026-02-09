#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	argc--;
	argv++;

	if (argc != 1) {
		exit(EXIT_FAILURE);
	}

	char *endptr;
	errno = 0;
	long val;

	val = strtol(argv[0], &endptr, 2);
	if (errno != 0 || *endptr != '\0') {
		exit(EXIT_FAILURE);
	}
	printf("%ld\n", val);

	exit(EXIT_SUCCESS);
}