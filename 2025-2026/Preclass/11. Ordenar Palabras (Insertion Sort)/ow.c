#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void
insertionsort(int num, char *cadena[]) {
    for (int i = 1; i < num; i++) {
        char *nodo = cadena[i];
        int j = i - 1;

        while (j >= 0 && strcmp(cadena[j], nodo) > 0) {
            cadena[j + 1] = cadena[j];
            j--;
        }

        cadena[j + 1] = nodo;
    }
}

int
main(int argc, char *argv[])
{
	argc--;
	argv++;

	if(argc < 1) {
		exit(EXIT_FAILURE);
	}

	insertionsort(argc, argv);
	for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

	exit(EXIT_SUCCESS);
}