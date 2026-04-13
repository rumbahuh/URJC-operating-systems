#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
myfgrep(int arguments, char * chain[])
{
	int numberofwordstosearch = arguments - 1;
	int status = 0;
	int argsofchain = arguments + 1 + 1 + 1 + numberofwordstosearch; // terminated in NULL + [0] is function name  + each word has -e
	int i = 0;
	int j = 2;

	char ** allocatedargs = malloc(sizeof(char*) * argsofchain); // allocated memory for array of strings
	if (allocatedargs == NULL) return 1;

	allocatedargs[0] = "fgrep";
	allocatedargs[1] = "-n";
	for (; i < numberofwordstosearch; i++) {
		allocatedargs[j++] = "-e";
		allocatedargs[j++] = chain[i];
	}
	allocatedargs[j++] = chain[numberofwordstosearch]; // filename
	allocatedargs[j] = NULL; // last index is total -1
	
	switch (fork()) {
	case -1:
		status = 1;
		break;
	case 0:
		execv("/bin/fgrep", allocatedargs);
		status = 1;
		free(allocatedargs);
		exit(1);
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

	return statusmain;
}