#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>

int
parsestring(char * stringnumber)
{
	int base = 10;
	char *endptr, *str;
	int val = 0;

	str = stringnumber;
	errno = 0;
	val = strtol(str, &endptr, base);

	if (errno == ERANGE) {
        perror("strtol");
        return -1;
    }

    if (endptr == str) {
        fprintf(stderr, "No digits were found\n");
        return -1;
    }

	return val;
}

int
calculatesubfiles(int nbytes, char * file, char * buffer)
{
	int nr;
	int current = 0;
	int nsubfiles = 0;

	while ((nr = read(current, buffer, nbytes)) != 0) {
		if (nr < 0) {
			fprintf(stderr, "can't read\n");
        	return -1;
		}
		current = current + nbytes;
		nsubfiles++;
	}
	
	return nsubfiles;
}

void
createnames(char ** buffernames, int filenumbers)
{
	int i = 0;
	char *current = "000";

	for(; i < filenumbers; i++) {
		buffernames[i] = current;
		// update current
	}
}

int
splitfile(char ** buffernames, int filenumbers, int nbytes, char * file)
{
	int i = 0;
	char * name;

	for(; i < filenumbers; i++) {
		name = buffernames[i];
		// concat name+file
		// createfile name+file
		// write nbytes from file in name+file
		(void)name;
	}

	return 0;
}

int
main(int argc, char* argv[])
{
	char * parentfile;
	int nbytes = 0;
	int subfiles = 0;
	char ** filenames;
	char * parentbuffer;
	int status = 0;

	argc--;
	argv++;

	if (argc != 2) {
		fprintf(stderr, "usage: mysplit N file\n");
		exit(EXIT_FAILURE);
	}

	parentfile = argv[1];

	nbytes = parsestring(argv[0]);
	if (nbytes == -1) {
		fprintf(stderr, "usage: mysplit N file\n");
		exit(EXIT_FAILURE);
	}
	parentbuffer = malloc(sizeof(char) * nbytes);

	subfiles = calculatesubfiles(nbytes, parentfile, parentbuffer);
	if (subfiles == -1) {
		fprintf(stderr, "usage: mysplit N file\n");
		free(parentbuffer);
		exit(EXIT_FAILURE);
	}

	filenames = malloc(sizeof(char*) * subfiles);
	createnames(filenames, subfiles);

	status = splitfile(filenames, subfiles, nbytes, parentfile);
	if (status == -1) {
			fprintf(stderr, "usage: mysplit N file\n");
			free(parentbuffer);
			free(filenames);
			exit(EXIT_FAILURE);
		}

	free(parentbuffer);
	free(filenames);
	exit(EXIT_SUCCESS);
}