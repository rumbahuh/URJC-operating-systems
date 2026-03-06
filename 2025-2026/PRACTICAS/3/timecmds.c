#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

enum {
    MAX_TOKENS_PER_ARG = 64
};

struct timecmd {
	char * cmd;
	pid_t num;
	int time;
	char * status;
};

int
tokenize(char *argument, char **buffer)
{
	int status = 0, i = 0;

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

	return status;
}

int
executeprogram(char ***argvpointers, int position, struct timecmd * chain)
{
	int status = 0;
	pid_t pid = 0;

	char *path = argvpointers[position][0];
	// already terminated in NULL in definition
	char **args = argvpointers[position];

	pid = fork();
	if (pid > 0) {
		chain[position].num = pid;
		chain[position].cmd = path;
	}

	switch(pid) {
	case -1:
		fprintf(stderr, "Fork failed\n");
		return -1;
	case 0:
		execv(path, args);
		fprintf(stderr, "Execution failed\n");
		status = -1;
		break;
	}

	return status;
}

int
main(int argc, char * argv[])
{
	int status = 0, i = 0, j = 0, k = 0, l = 0;
	argc--;
	argv++;

	struct timecmd *timecmds = malloc(sizeof(struct timecmd) * argc);
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
		status = executeprogram(argvptrs, k, timecmds);
		if (status) {
			timecmds[k].status = "success";
		} else {
			timecmds[k].status = "failure";
		}
    }

	for(; j < argc; j++) {
		pid_t finished_pid = waitpid(timecmds[j].num, &status, 0);
		printf("cmd: %s, pid: %d, status: %s\n", timecmds[j].cmd, finished_pid, timecmds[j].status);
	} 

	for (; l < argc; l++) {
		free(argvptrs[l]);
		free(copyofargv[l]);
	}
	free(argvptrs);
	free(copyofargv);
	free(timecmds);

	return status;
}