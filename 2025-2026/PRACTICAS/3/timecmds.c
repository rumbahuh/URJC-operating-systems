#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
tokenize(char * argument)
{
	int localstatus = 0, i = 0;;

	// from strtok_r(3) man page
	char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;
	const char *restrict delim = " ";

    for (str1 = argument; ; i++, str1 = NULL) {
        token = strtok_r(str1, delim, &saveptr1);
        if (token == NULL)
            break;
        printf("%d: %s\n", i, token);

        for (str2 = token; ; str2 = NULL) {
            subtoken = strtok_r(str2, delim, &saveptr2);
            if (subtoken == NULL)
                break;
            printf("\t --> %s\n", subtoken);
            }
        }

	return localstatus;
}

int
main(int argc, char * argv[])
{
	int status = 0, i = 0, j = 0;
	argc--;
	argv++;

	for(; i < argc; i++) {
		status = tokenize(argv[i]);
	}
	
	// We check new argv structure
	for(; j < argc; j++) {
		fprintf(stdout, "%s\n", argv[j]);
	}
	
	return status;
}