#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int
parsestring(char * stringnumber)
{
	int base = 10;
	char *endptr, *str;
	int val = 0;

	str = stringnumber;
	errno = 0;
	val = strtol(str, &endptr, base);

	if (errno == ERANGE) {
        perror("strtol");
        return -1;
    }

    if (endptr == str) {
        fprintf(stderr, "No digits were found\n");
        return -1;
    }

	return val;
}

int
main(int argc, char* argv[])
{
	char * parentfile;
	int nbytes = 0;

	argc--;
	argv++;

	if (argc != 2) {
		fprintf(stderr, "usage: ./mysplit [Nbytes] [file]\n");
		exit(EXIT_FAILURE);
	}

	parentfile = argv[1];
	(void)parentfile;

	nbytes = parsestring(argv[0]);
	if (nbytes == -1) {
		fprintf(stderr, "usage: ./mysplit [Nbytes] [file]\n");
		fprintf(stderr, "Nbytes must be a number\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}