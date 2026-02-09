#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

void checkpori(int numero)
{
	if ((numero % 2) == 0) {
		printf("numero es par\n");
	} else {
		printf("numero es impar\n");
	}
}

int main(int argc, char *argv[])
{
	argc--;
	argv++;

	if (argc == 1) {
		char *endptr;
        long value;

        errno = 0;
        value = strtol(argv[0], &endptr, 10);

        /* Check for conversion errors */
        if (errno != 0 || *endptr != '\0') {
            /* Not a valid number */
            exit(EXIT_FAILURE);
        }
		
		checkpori(value);
		
	} else {
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}