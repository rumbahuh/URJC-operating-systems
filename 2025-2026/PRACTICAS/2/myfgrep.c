#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
myfgrep(int arguments, char * chain[])
{
	int numberofwordstosearch = arguments - 1;
	int status = 0;

	int argsofchain = arguments + 1 + 1; // terminated in NULL plus [0] is function name
	char ** allocatedargs = malloc(sizeof(char*) * argsofchain); // allocated memory for array of strings
	int i = 0;

	allocatedargs[0] = "fgrep";
	for (; i < arguments; i++) {
		allocatedargs[i + 1] = chain[i];
	}
	allocatedargs[argsofchain - 1] = NULL; // last index is total -1

	(void)numberofwordstosearch;
	switch (fork()) {
	case -1:
		status = 1;
		break;
	case 0:
		execv("/bin/fgrep", allocatedargs); // no está buscando las que yo quiero solo la primera como pattern
		status = 1;
		break;
	}

	free(allocatedargs);
	return status;
}

int
main(int argc, char * argv[])
{
	int statusmain = 0;
	argc--;
	argv++;

	if (argc < 2) {
		fprintf(stderr, "usage: myfgrep word [word]... file\n");
		return 1;
	}

	statusmain = myfgrep(argc, argv);
	if (statusmain) fprintf(stderr, "file grep failure\n");
	printf("am i alive?\n\n");

	return statusmain;
}