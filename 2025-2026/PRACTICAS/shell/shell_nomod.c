#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

enum {
	MAX_BYTES_LINEA = 512,
	MAX_BYTES_ARG = 400
};

/* Estructura que guarda la información
   de redirección de un comando. */
struct RedirInfo {
	char *command;
	char *file_in;
	char *file_out;
	int background;
};

typedef struct RedirInfo RedirInfo;

/* Estructura que guarda una RedirInfo
   además de los argumentos tokenizados
   para un comando. */
struct Tokenized {
	RedirInfo structure;
	char **arguments;
	int numarguments;
	int specncom;
};

typedef struct Tokenized Tokenized;

/*
 * Quita los espacios, tabulados o espacios de linea
 */
char *
trim(char *str)
{
	char *end;

	while (*str == ' ' || *str == '\t' || *str == '\n')
		str++;

	if (*str == '\0')
		return str;

	end = str + strlen(str) - 1;
	while (end > str && (*end == ' ' || *end == '\t' || *end == '\n'))
		*end-- = '\0';

	return str;
}

/* 
 * Cambia de directorio, si no se pasa
 * path, recoge home y cambia a ese.
 */
void
builtincd(char *args[], int arg)
{
	const char *home = getenv("HOME");

	if (arg == 1) {
		if (home == NULL) {
			fprintf(stderr, "HOME environment variable not set.\n");
			return;
		}

		if (chdir(home) != 0) {
			perror("chdir to HOME failed");
		}

		return;
	}

	if (chdir(args[1]) != 0) {
		perror("cd failed");
	}
}

/*
 * Hace fork para poder remplazar proceso hijo con el comando pasado a ejecutar.
 * Previamente comprueba las redirecciones especificadas.
 * Espera en un bucle los estados si es que no se le hubiera pasado estado de background true.
 */
void
executefile(RedirInfo rinfo, char fullpath[], char *path, char *args[], int arg)
{
	int sts, pid;
	int fd_in = 0, fd_out = 0, fd;
    int pid_recogido = 0;

	pid = fork();
	switch (pid) {
	case -1:
		err(EXIT_FAILURE, "fork failed!");
		break;

	case 0:
		if (rinfo.background && rinfo.file_in == NULL) {
			fd = open("/dev/null", O_RDONLY);
			if (fd < 0)
				err(EXIT_FAILURE, "cant open /dev/null");
			dup2(fd, STDIN_FILENO);
			close(fd);
		}

		if (rinfo.background && rinfo.file_out == NULL) {
			fd = open("/dev/null", O_WRONLY);
			if (fd < 0)
				err(EXIT_FAILURE, "can't open /dev/null");
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}

		if (rinfo.file_in) {
			fd_in = open(rinfo.file_in, O_RDONLY);
			if (fd_in < 0)
				err(EXIT_FAILURE, "can't open input file");
			dup2(fd_in, STDIN_FILENO);
			close(fd_in);
		}

		if (rinfo.file_out) {
			fd_out =
			    open(rinfo.file_out, O_WRONLY | O_CREAT | O_TRUNC,
				 0644);
			if (fd_out < 0)
				err(EXIT_FAILURE, "can't open output file");
			dup2(fd_out, STDOUT_FILENO);
			close(fd_out);
		}

		if (fullpath != NULL) {
			execv(fullpath, args);
		} else {
			execv(path, args);
		}

		err(EXIT_FAILURE, "execv failed");
		break;

	default:
		if (!rinfo.background) {
			while ((pid_recogido = wait(&sts)) > 0) {
                if (pid_recogido == pid) {
                    break;
                } else {
                    // This was some other child (e.g., a background process that died)
                    // Reap it to avoid zombies, but don't block on it.
                    fprintf(stderr, "Proceso en bg muerto y recogido: %d\n", pid_recogido);
                }
			}
		}
		break;
	}
}

/*
 * Verifica si una línea contiene una asignación de variable
 * y la establece como variable de entorno.
 * Asigna null al final de los strings.
 * 
 * Devuelve 1 si hay espacios entre el igual,
 * 2 si no encuentra el igual para una asignación
 * válida, o devuelve 0.
 */
int
variablescheck(char *array[], char *line, int arg, char char_a_buscar)
{
	char *puntero_a_ocurrencia_de_signo;
	char entorno[MAX_BYTES_ARG], valor[MAX_BYTES_ARG];
	int status;
	int i = 0;

	if (arg > 1) {
		for (; i < arg; i++) {
			puntero_a_ocurrencia_de_signo = strchr(array[i], char_a_buscar);
			if (puntero_a_ocurrencia_de_signo != NULL) {
				return 1;
			}
		}
	}

	puntero_a_ocurrencia_de_signo = strchr(line, char_a_buscar);
	if (puntero_a_ocurrencia_de_signo == NULL) {
		return 2;
	}

	*puntero_a_ocurrencia_de_signo = '\0';
	strncpy(entorno, line, MAX_BYTES_ARG);
	entorno[MAX_BYTES_ARG - 1] = '\0';

	// Guardamos el valor de la variable
	strncpy(valor, puntero_a_ocurrencia_de_signo + 1, MAX_BYTES_ARG);
	valor[MAX_BYTES_ARG - 1] = '\0';

	status = setenv(entorno, valor, 1);
	if (status != 0) {
		err(EXIT_FAILURE, "impossible to set environment variable");
	}

	return 0;
}

/*
 * Separa el comando y sus argumentos por espacios.
 * Devuelve una estructura del tipo Tokenized.
 */
Tokenized
tokenizecommand(RedirInfo input)
{
	Tokenized structure_;
	char *str, *token, *saveptr;
	char **args = malloc(sizeof(char *) * MAX_BYTES_ARG);
	int arg = MAX_BYTES_ARG;
	int i = 0;

	if (!args) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	for (str = input.command;; str = NULL) {
		token = strtok_r(str, " ", &saveptr);
		if (token == NULL)
			break;

		token = trim(token);
		if (strlen(token) == 0)
			continue;

		if (i < arg - 1) {
			args[i++] = strdup(token);
			if (!args[i - 1]) {
				perror("strdup failed");
				exit(EXIT_FAILURE);
			}
		}
	}

	args[i] = NULL;
	arg = i;

	structure_.arguments = args;
	structure_.numarguments = arg;
	structure_.structure = input;

	return structure_;
}

/*
 * Determina si el comando es 'cd', ejecutable directo, 
 * está en el PATH, o si es una variable.
 * Si se estuviera pidiendo el valor de la variable (ej: $x)
 * lo imprimiría por standard.
 */
void
iscommand(RedirInfo rinfo, int arg, char *args[])
{
	char *path, *remains, *dir, *saveptr, fullpath[MAX_BYTES_LINEA];

	// eliminamos los $ de los argumentos
	if (strcmp(rinfo.command, "cd") == 0) {
		builtincd(args, arg);
		return;
	} else if (access(rinfo.command, X_OK) == 0) {
		executefile(rinfo, NULL, rinfo.command, args, arg);
		return;
	} else {
		path = getenv("PATH");
		remains = strdup(path);
		if (remains == NULL) {
			perror("strdup failed");
			return;
		}

		dir = strtok_r(remains, ":", &saveptr);

		while (dir != NULL) {
			strcpy(fullpath, dir);
			if (dir[strlen(dir) - 1] != '/') {
				strcat(fullpath, "/");
			}
			strcat(fullpath, rinfo.command);
			if (access(fullpath, X_OK) == 0) {
				args[arg] = NULL;
				executefile(rinfo, fullpath, rinfo.command,
					    args, arg);
				free(remains);
				return;
			}
			dir = strtok_r(NULL, ":", &saveptr);
		}
		free(remains);
		printf("%s: command not found\n", args[0]);
	}
}

/*
 * Comprueba que $ char esté presente en la cadena.
 * Si no está, libera, asigna y devuelve.
 * De otra manera, cambia los argumentos por las variables locales que contienen.
 * Si ha habido error, libera.
 * Finalmente devuelve.
 */
Tokenized
ejecuciondevariables(Tokenized tinfo)
{
	char **args_ = calloc(tinfo.numarguments + 1, sizeof(char *));
	char *val, *chartolook;
	Tokenized tinfo_;
	RedirInfo rinfo_;
	int i = 0, found = 0;

	for (int d = 0; d < tinfo.numarguments; d++) {
		chartolook = strchr(tinfo.arguments[d], '$');
		if (chartolook != NULL) {
			found = 1;
			break;
		}
	}

	if (!found) {
		tinfo_.specncom = 2;
		tinfo_ = tinfo;
		free(args_);
		return tinfo_;
	}

	tinfo_.specncom = 0;
	for (; i < tinfo.numarguments; i++) {
		if (tinfo.arguments[i][0] == '$') {
			val = getenv(tinfo.arguments[i] + 1);
			if (val) {
				args_[i] = strdup(val);
				if (!args_[i]) {
					perror("strdup failed");
					tinfo_.specncom = 1;
					break;
				}
			} else {
				fprintf(stderr, "error: var %s does not exist\n", tinfo.arguments[i]);
				tinfo_.specncom = 1;
				break;
			}
		} else {
			args_[i] = strdup(tinfo.arguments[i]);
			if (!args_[i]) {
				perror("strdup failed");
				tinfo_.specncom = 1;
				break;
			}
		}

		// This must be outside both branches
		if (i == 0) {
    		rinfo_.command = args_[0];
    		rinfo_.file_in = NULL;
    		rinfo_.file_out = NULL;
    		rinfo_.background = 0;
		}
	}

	if (tinfo_.specncom == 1) {
		for (int j = 0; j <= i; j++) {
			if (args_[j]) free(args_[j]);
		}
		free(args_);
		return tinfo;
	}

	tinfo_.arguments = args_;
	tinfo_.numarguments = tinfo.numarguments;
	tinfo_.structure = rinfo_;

	return tinfo_;
}


/* 
 * Llama a variablescheck.
 * si hay un igual y son diferentes argumentos, es un error.
 * Devuelve status
 */
int
asignaciondevariables(RedirInfo rinfo, int arg, char *args[])
{
	int status;
	status = variablescheck(args, rinfo.command, arg, '=');
	if (arg > 1 && status == 1) {
		fprintf(stderr,
			"error: wrong variable assignation, try x=y\n");
	}

	return status;
}

/*
 * Verifica si el comando debe ejecutarse en segundo plano 
 * (si contiene '&') y elimina el carácter.
 */
char *
backgroundcheck(char linea[])
{
	size_t new_size = 0;
	char *newline, *firstcharamper;

    firstcharamper = strchr(linea, '&');
    if (firstcharamper != NULL) {
		// actualizamos el pointer
        new_size = firstcharamper - linea;
        newline = strndup(linea, new_size);
        return trim(newline);
    }

    return linea;
}


RedirInfo
redirectioncheck(char linea[])
{
	RedirInfo rinfo = { NULL, NULL, NULL };
	char *token, *saveptr;
	char *redir = linea;
	int j = 0;
	const char *delims = "<>";
	char last_delim = 0;

	// Hago una copia de la línea
	char *line_copy = strdup(linea);
	char *p = line_copy;

	while ((token = strtok_r(redir, delims, &saveptr)) != NULL) {
		redir = NULL;

		// Quitamos espacios del token
		token = trim(token);

		if (j == 0) {
			rinfo.command = token;
		} else {
			// Pasamos a después del delimitador
			while (*p && *p != '<' && *p != '>')
				p++;
			if (*p) {
				last_delim = *p;
				p++;
			}

			if (last_delim == '<') {
				rinfo.file_in = token;
			} else if (last_delim == '>') {
				rinfo.file_out = token;
			}
		}

		j++;
	}

	free(line_copy);

	return rinfo;
}

int
main(int argc, char *argv[])
{
	char *line = malloc(MAX_BYTES_LINEA * sizeof(char));
	char *linebg = NULL;
	char hostname[MAX_BYTES_LINEA];
	char cwd[MAX_BYTES_LINEA];
	char *username;
	int background;
	RedirInfo rinfo;
	Tokenized tinfo;
	Tokenized tinfodup;
	int status;

	if (argc > 1) {
		fprintf(stderr, "wrong num of args\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		background = 0;
		getcwd(cwd, sizeof(cwd));
		username = getlogin();
		gethostname(hostname, sizeof(hostname));

		fprintf(stderr, "\033[1;36m%s@%s\033[0m:\033[1;35m%s\033[0m$ ", username,
		       hostname, cwd);
		if (fgets(line, MAX_BYTES_LINEA, stdin) == NULL) {
			break;
		}

		if (line[0] == '\n')
			continue;

		linebg = backgroundcheck(line);
		if (strcmp(line, linebg) != 0) {
			background = 1;
		}

		rinfo = redirectioncheck(linebg);
		rinfo.background = background;

		if (rinfo.command != NULL) {
			tinfo = tokenizecommand(rinfo);
			status = asignaciondevariables(tinfo.structure, tinfo.numarguments, tinfo.arguments);
			if (status == 2) {
				tinfodup = ejecuciondevariables(tinfo);
				if (tinfodup.specncom == 0) {
					iscommand(tinfodup.structure,
					tinfodup.numarguments, tinfodup.arguments);
							
					if (tinfodup.arguments != tinfo.arguments) {
    					for (int j = 0; j < tinfodup.numarguments; j++) {
        					free(tinfodup.arguments[j]);
    					}
    					free(tinfodup.arguments);
					}
				}
			}

			for (int i = 0; i < tinfo.numarguments; i++) {
						free(tinfo.arguments[i]);
					}
					free(tinfo.arguments);
		}
	}

	if (!feof(stdin)) {
		errx(EXIT_FAILURE, "eof not reached");
	}

	// liberamos porque backgroundcheck
	// hace un strdup y devuelve en linebg
	if (linebg != line) {
    free(linebg);
	}
	free(line);

	exit(EXIT_SUCCESS);
}
