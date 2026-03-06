#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    MAX_TOKENS_PER_ARG = 64
};

int
tokenize(char *argument, char **buffer)
{
	int localstatus = 0, i = 0;

	// from strtok_r(3) man page
	char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;
	const char *restrict delim = " ";

    for (str1 = argument; ; str1 = NULL) {
        token = strtok_r(str1, delim, &saveptr1);
        if (token == NULL)
            break;

        for (str2 = token; ; str2 = NULL) {
            subtoken = strtok_r(str2, delim, &saveptr2);
            if (subtoken == NULL)
                break;
			if (i < MAX_TOKENS_PER_ARG - 1) {
                buffer[i] = subtoken;
                i++;
            }
        }
    }
	buffer[i] = NULL;

	return localstatus;
}

int
main(int argc, char * argv[])
{
	int status = 0, i = 0, j = 0, k = 0;
	argc--;
	argv++;

	// array of arrays of strings
	char ***argvptrs = malloc(sizeof(char **) * argc);
	char **copyofargv = malloc(sizeof(char *) * argc);
	if (copyofargv == NULL || argvptrs == NULL) return 1;
	for(; i < argc; i++) {
		argvptrs[i] = malloc(sizeof(char *) * MAX_TOKENS_PER_ARG);
		copyofargv[i] = malloc(strlen(argv[i]) + 1);
		strcpy(copyofargv[i], argv[i]);
		tokenize(copyofargv[i], argvptrs[i]);
	}
	
	for (; k < argc; k++) {
        printf("Shelf %d content:\n", i);
        for (int l = 0; argvptrs[k][l] != NULL; l++) {
            printf("  Slot %d: %s\n", l, argvptrs[k][l]);
        }
    }

	for (; j < argc; j++) {
		free(argvptrs[j]);
		free(copyofargv[j]);
	}
	free(argvptrs);
	free(copyofargv);

	return status;
}