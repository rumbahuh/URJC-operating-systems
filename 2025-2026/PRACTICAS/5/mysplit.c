#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>

/* 
 * Passes string to int using strtol.
 * Handles errors.
 */
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

/* 
 * Reads bytes on file.
 * Every nbytes it adds to nsubfiles
 * which is a counter and updates the
 * byte position of total of file bytes.
 */
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

/* 
 * For each filenumbers it creates a char id,
 * starting at 000, 001, etc,.
 * Adds it to buffernames.
 */
void
createnames(char ** buffernames, int filenumbers, int length, char * filename)
{
	int i = 0;

	for(; i < filenumbers; i++) {
        snprintf(buffernames[i], length, "%03d%s", i, filename);
	}
}

/* 
 * Creates file and write bytes from file up to Nbytes.
 */
int
splitfile(char ** buffernames, int filenumbers, int nbytes, char * file)
{
	int i = 0;
	char * name;

	for(; i < filenumbers; i++) {
		name = buffernames[i];
		// createfile name
		// write nbytes from file in name
		(void)name;
	}

	return 0;
}

void
freeall(char * parent, char ** files, int number)
{
	int i = 0;

	free(parent);
	for(; i < number; i++) {
		free(files[i]);
	}
	free(files);
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
	int len = 0;
	int j = 0;

	argc--;
	argv++;

	if (argc != 2) {
		fprintf(stderr, "usage: mysplit N file\n");
		exit(EXIT_FAILURE);
	}

	parentfile = argv[1];
	len = strlen("000") + strlen(parentfile) + 1; // + '\0'

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
	for(; j < subfiles; j++) {
		filenames[j] = malloc(sizeof(char) * len);
	}
	createnames(filenames, subfiles, len, parentfile);

	status = splitfile(filenames, subfiles, nbytes, parentfile);
	if (status == -1) {
			fprintf(stderr, "usage: mysplit N file\n");
			freeall(parentbuffer, filenames, subfiles);
			exit(EXIT_FAILURE);
		}

	freeall(parentbuffer, filenames, subfiles);
	exit(EXIT_SUCCESS);
}