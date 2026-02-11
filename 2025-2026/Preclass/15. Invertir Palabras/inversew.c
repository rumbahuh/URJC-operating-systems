#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
inverseword(int num, char * cadena[])
{
	int i = 0;
	for(; i < num; i++) {
		char *word = cadena[i];
		int n = strlen(word);

		char * inversew = malloc(sizeof(char) * (n + 1));
		if (inversew == NULL) return;

		for (int j = 0; j < n; j++) {
            inversew[j] = word[n - 1 - j];
        }
		inversew[n] = '\0';

		printf("%s\n", inversew);
		free(inversew);
	}
}

int
main(int argc, char * argv[])
{
	if (argc < 2) {
        exit(EXIT_FAILURE);
    }

    inverseword(argc - 1, &argv[1]);
    
    exit(EXIT_SUCCESS);
}