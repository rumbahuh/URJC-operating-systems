#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

enum {
	Maxline = 512,
};

int
buffering(int fd)
{
	char line[Maxline];
	int status = 0;
	FILE *f;

	f = fdopen(fd, "r");
	if (f == NULL) {
		err(EXIT_FAILURE, "cant fopen fd");
	}
	while (fgets(line, sizeof(line), f) != NULL) {
		printf("Línea %s", line);
	}

	fclose(f);
	return status;
}

int
main(int argc, char* argv[])
{
	char line[Maxline];
	//int numberofurls = 0;
	//int index = 0;
	int fd;
	int n;
	int statusnumber = 0;

	argc--;
	argv++;

	if (argc > 1) {
		fprintf(stderr, "error, invalid arguments\n");

	} else if (argc == 1) {
		fd = open(argv[0], O_RDONLY);
		if (fd < 0) {
			err(EXIT_FAILURE, "cannot open file");
		}
		n = buffering(fd);
		if (n != 0) {
			statusnumber++;
			fprintf(stderr, "%d err number. couldnt download\n", statusnumber);
		}

	} else {
		while (fgets(line, Maxline, stdin) != NULL) {
			printf("line: %s\n", line);
		}
		if (!feof(stdin)) {
			errx(EXIT_FAILURE, "eof not reached");
		}
	}
	
	return statusnumber;
}