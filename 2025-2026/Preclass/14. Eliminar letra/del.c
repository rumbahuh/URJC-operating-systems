#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void
delletter(int num, char * lista[], char letra, char ** arr)
{
	int i = 0;
	for(; i < num; i++) {
		char *src = lista[i];

		arr[i] = malloc(strlen(src) + 1);

		int j = 0;
		for(int k = 0; src[k] != '\0'; k++) {
			char key = src[k];
			if (key != letra) {
				arr[i][j++] = key;
			}
		}
		arr[i][j] = '\0';
	}
}

int
main(int argc, char * argv[])
{
	argc--;
	argv++;

	if (argc < 1) {
		exit(EXIT_FAILURE);
	}
	char ** arr = malloc(sizeof(char*) * argc);

	delletter(argc-1, argv, argv[argc-1][0], arr);
	for (int i = 0; i < argc-1; i++) {
        printf("%s\n", arr[i]);
        free(arr[i]);
    }

	exit(EXIT_SUCCESS);
}