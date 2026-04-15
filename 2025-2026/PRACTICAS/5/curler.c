#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

enum {
	Maxline = 512,
};

int
checkline(char * linea)
{
	char * valid1 = "http://";
	char * valid2 = "https://";
	int status = 0;

	if (strncmp(linea, valid1, 7) == 0 || strncmp(linea, valid2, 8) != 0) {
		status = 1;
		fprintf(stderr, "error: invalid line 'patata'\n");
	}

	return status;
}

int
download(char *linea)
{
    int status = 0;
    pid_t pid;

    switch(pid = fork()) {
    case -1:
        err(EXIT_FAILURE, "fork failed!");
    case 0:
        execl("/usr/bin/curl", "curl", "--connect-timeout", "5", linea, NULL);
        err(EXIT_FAILURE, "execl failed");
        break;
    default:
        if (waitpid(pid, &status, 0) < 0)
            err(EXIT_FAILURE, "waitpid failed");
        status = WEXITSTATUS(status);
        break;
    }
    return status;
}

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
		line[strcspn(line, "\n")] = '\0';
		status = checkline(line);
		if (checkline(line) != 0) {
            fprintf(stderr, "error: invalid line \"%s\"\n", line);
            status = 255;
        } else {
            if (download(line) != 0)
                status = 1;
        }
	}

	fclose(f);
	return status;
}

int
main(int argc, char* argv[])
{
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
		}

	} else {
		n = buffering(STDIN_FILENO);  // fd 0 = stdin
		if (n != 0) {
			statusnumber++;
		}
	}
	
	return statusnumber;
}