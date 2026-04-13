#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

enum {
	MAX_TOKENS_PER_ARG = 64
};

struct timecmd {
	char *fullstring;
	// char *cmd;
	pid_t num;
	time_t start;
	char *status;
};

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
executeprogram(char ***argvpointers, int position, struct timecmd *chain)
{
	pid_t pid = 0;

	char *path = argvpointers[position][0];

	// already terminated in NULL in definition
	char **args = argvpointers[position];

	chain[position].start = time(NULL);
	pid = fork();
	if (pid > 0) {
		chain[position].num = pid;
		// chain[position].cmd = path;
	}

	switch (pid) {
	case -1:
		fprintf(stderr, "Fork failed\n");
		return -1;
	case 0:
		execv(path, args);
		exit(1);
	}

	return 0;
}

void
forkall(char ***argvptrs, struct timecmd *timecmds, char **argv, int argc)
{
	for (int k = 0; k < argc; k++) {
		timecmds[k].fullstring = argv[k];
		executeprogram(argvptrs, k, timecmds);
	}
}

int
waitall(struct timecmd *timecmds, int argc)
{
	int status = 0;

	for (int j = 0; j < argc; j++) {
		int wstatus = 0;
		pid_t finished_pid = wait(&wstatus);
		time_t end = time(NULL);

		struct timecmd *cmd = NULL;

		for (int m = 0; m < argc; m++) {
			if (timecmds[m].num == finished_pid) {
				cmd = &timecmds[m];
				break;
			}
		}
		if (cmd == NULL)
			continue;

		int elapsed = (int)(end - cmd->start);

		if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0) {
			cmd->status = "success";
		} else {
			cmd->status = "failure";
			status = 1;
		}
		printf("cmd: %s, pid: %d, time: %d seconds, status: %s\n",
		       cmd->fullstring, finished_pid, elapsed, cmd->status);
	}

	return status;
}

void
freeall(char ***argvptrs, char **copyofargv, int argc, struct timecmd *timecmds)
{
	for (int l = 0; l < argc; l++) {
		free(argvptrs[l]);
		free(copyofargv[l]);
	}
	free(argvptrs);
	free(copyofargv);
	free(timecmds);
}

int
main(int argc, char *argv[])
{
	int status = 0, i = 0;

	argc--;
	argv++;

	struct timecmd *timecmds = malloc(sizeof(struct timecmd) * argc);

	// array of arrays of strings
	char ***argvptrs = malloc(sizeof(char **) * argc);
	char **copyofargv = malloc(sizeof(char *) * argc);

	if (copyofargv == NULL || argvptrs == NULL || timecmds == NULL)
		return 1;

	for (; i < argc; i++) {
		argvptrs[i] = malloc(sizeof(char *) * MAX_TOKENS_PER_ARG);
		copyofargv[i] = malloc(strlen(argv[i]) + 1);
		strcpy(copyofargv[i], argv[i]);
		tokenize(copyofargv[i], argvptrs[i]);
	}

	forkall(argvptrs, timecmds, argv, argc);
	status = waitall(timecmds, argc);

	freeall(argvptrs, copyofargv, argc, timecmds);

	return status;
}
