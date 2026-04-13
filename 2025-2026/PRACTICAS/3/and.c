#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

enum {
	MAX_TOKENS_PER_ARG = 64
};

int
getrealsize(char *chain)
{
	int i = 0, counter = 1;
	char del = ' ';

	for (; i < strlen(chain); i++) {
		if (chain[i] == del)
			counter++;
	}

	return counter;
}

int
tokenize(char *argument, char **buffer)
{
	int status = 0, i = 0;

	// from strtok_r(3) man page
	char *str1, *str2, *token, *subtoken;
	char *saveptr1, *saveptr2;
	const char *restrict delim = " ";

	for (str1 = argument;; str1 = NULL) {
		token = strtok_r(str1, delim, &saveptr1);
		if (token == NULL)
			break;

		for (str2 = token;; str2 = NULL) {
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

	return status;
}

int
hijoconcurrente(char *fullcmd, char *args[])
{
	int sts = 0;
	int status = 0;
	int pid = 0;

	pid = fork();
	switch (pid) {
	case -1:
		fprintf(stderr, "bad fork\n");
		return 1;
	case 0:
		execv(fullcmd, args);
		fprintf(stderr, "execv failed\n");
		exit(EXIT_FAILURE);
		break;
	}

	while ((pid = wait(&sts)) != -1) {
		if (WIFEXITED(sts)) {
			return WEXITSTATUS(sts);
		} else {
			status = 1;
		}
	}

	return status;
}

int
main(int argc, char *argv[])
{
	int i = 0, status = 0, size = 0;
	char **tmparray;

	argc--;
	argv++;

	if (argc < 2) {
		fprintf(stderr, "usage: and cmd cmd [cmd ...]\n");
		return 1;
	}

	do {
		char specpath[256];

		strcpy(specpath, "/bin/");

		size = getrealsize(argv[i]);
		tmparray = malloc(sizeof(char *) * (size + 1));	// + 1 cause of NULL
		tokenize(argv[i], tmparray);

		strcat(specpath, tmparray[0]);
		status = hijoconcurrente(specpath, tmparray);
		free(tmparray);
		if (status != 0) {
			return status;
		}

		i++;
	} while (status != 1 && i < argc);

	return EXIT_SUCCESS;
}
