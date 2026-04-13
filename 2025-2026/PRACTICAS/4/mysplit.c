#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>

/* 
 * Passes string to int using strtol.
 * Handles errors.
 */
int
parsestring(char *stringnumber)
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
calculatesubfiles(int nbytes, char *file, char *buffer)
{
	int nr;
	int nsubfiles = 0;
	int fd;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "can't open %s\n", file);
		return -1;
	}

	while ((nr = read(fd, buffer, nbytes)) > 0)
		nsubfiles++;
	if (nr < 0) {
		fprintf(stderr, "can't read\n");
		close(fd);
		return -1;
	}

	close(fd);
	return nsubfiles;
}

/* 
 * For each filenumbers it creates a char id,
 * starting at 000, 001, etc,.
 * Adds it to buffernames.
 */
void
createnames(char **buffernames, int filenumbers, int length, char *filename)
{
	int i = 0;

	for (; i < filenumbers; i++) {
		snprintf(buffernames[i], length, "%03d%s", i, filename);
	}
}

/* 
 * Creates file and write bytes from file up to Nbytes.
 */
int
splitfile(char **buffernames, int filenumbers, int nbytes, char *file,
	  char *buffer)
{
	int i = 0;
	int fd_in, fd_out;
	int nr, nw, written;

	fd_in = open(file, O_RDONLY);
	if (fd_in < 0) {
		fprintf(stderr, "can't open %s\n", file);
		return -1;
	}

	for (; i < filenumbers; i++) {
		fd_out = open(buffernames[i], O_WRONLY | O_CREAT | O_TRUNC, 0664);	// TRUNC is an spec

		if (fd_out < 0) {
			fprintf(stderr, "can't open %s\n", buffernames[i]);
			close(fd_in);
			return -1;
		}
		nr = read(fd_in, buffer, nbytes);
		if (nr < 0) {
			fprintf(stderr, "can't read\n");
			close(fd_out);
			close(fd_in);
			return -1;
		}

		written = 0;

		while (written < nr) {
			nw = write(fd_out, buffer + written, nr - written);
			if (nw < 0) {
				fprintf(stderr, "can't write\n");
				close(fd_out);
				close(fd_in);
				return -1;
			}
			written += nw;
		}
		close(fd_out);
	}
	close(fd_in);
	return 0;
}

void
freeall(char *parent, char **files, int number)
{
	int i = 0;

	if (files != NULL) {
		for (; i < number; i++) {
			free(files[i]);
		}
		free(files);
	}
	free(parent);
}

int
main(int argc, char *argv[])
{
	char *parentfile;
	int nbytes = 0;
	int subfiles = 0;
	char **filenames;
	char *parentbuffer;
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
	len = strlen("000") + strlen(parentfile) + 1;	// + '\0'

	nbytes = parsestring(argv[0]);
	if (nbytes == -1) {
		fprintf(stderr, "usage: mysplit N file\n");
		exit(EXIT_FAILURE);
	}
	if (nbytes <= 0)
		exit(EXIT_FAILURE);

	parentbuffer = malloc(sizeof(char) * nbytes);
	if (parentbuffer == NULL) {
		fprintf(stderr, "bad malloc");
		exit(EXIT_FAILURE);
	}

	subfiles = calculatesubfiles(nbytes, parentfile, parentbuffer);
	if (subfiles == -1) {
		fprintf(stderr, "usage: mysplit N file\n");
		free(parentbuffer);
		exit(EXIT_FAILURE);
	}

	filenames = calloc(subfiles, sizeof(char *));
	if (filenames == NULL) {
		fprintf(stderr, "bad malloc");
		free(parentbuffer);
		exit(EXIT_FAILURE);
	}

	for (; j < subfiles; j++) {
		filenames[j] = malloc(len);
		if (filenames[j] == NULL) {
			fprintf(stderr, "bad malloc");
			freeall(parentbuffer, filenames, j);
			exit(EXIT_FAILURE);
		}
	}
	createnames(filenames, subfiles, len, parentfile);

	status =
	    splitfile(filenames, subfiles, nbytes, parentfile, parentbuffer);
	if (status == -1) {
		fprintf(stderr, "usage: mysplit N file\n");
		freeall(parentbuffer, filenames, subfiles);
		exit(EXIT_FAILURE);
	}

	freeall(parentbuffer, filenames, subfiles);
	exit(EXIT_SUCCESS);
}
