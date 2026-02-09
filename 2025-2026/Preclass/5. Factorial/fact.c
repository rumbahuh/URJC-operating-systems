#include <stdlib.h>
#include <stdio.h>

int fact(int num) {
	int i = num;
	int fact = 1;

	for (; i > 0; i--) {
		fact = fact * i;
	}

	return fact;
}

int main(int argc, char *argv[])
{
	argc--;
	argv++;
	if (argc != 1) {
		exit(EXIT_FAILURE);
	}

	char *endptr;
	long val;
	int errno = 0;

	val = strtol(argv[0], &endptr, 10);

	if (errno != 0 || *endptr != '\0') {
		exit(EXIT_FAILURE);
	}

	int factorial = fact(val);
	printf("%d\n", factorial);

	exit(EXIT_SUCCESS);
}