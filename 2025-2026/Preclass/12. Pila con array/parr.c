#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
	argc--;
	argv++;
	int i = 0;
	int j = argc - 1;

	if (argc < 1) {
		exit(EXIT_FAILURE);
	}

	char ** arr = malloc(sizeof(char) * argc);
	
	for (; i < argc; i++) {
		arr[i] = argv[i];
	}

	for (; j >= 0; j--) {
		printf("%s\n", arr[j]);
	}

	free(arr);
	exit(EXIT_SUCCESS);
}