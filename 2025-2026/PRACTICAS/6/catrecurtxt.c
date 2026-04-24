#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

/*
   Explora dirpath de forma recursiva y vuelca el contenido
   de cada .txt que encuentre en outfd.
*/
static void
explorar_carpeta(const char *dirpath, int outfd)
{
	DIR *carpeta = opendir(dirpath);

	if (carpeta == NULL)
		err(EXIT_FAILURE, "No puedo abrir la carpeta: %s", dirpath);

	struct dirent *entrada;	// cada cosa que hay dentro de la carpeta
	struct stat info;	// información sobre esa cosa

	while ((entrada = readdir(carpeta)) != NULL) {

		// saltamos "." y ".." o entraríamos en bucle infinito
		if (strcmp(entrada->d_name, ".") == 0 ||
		    strcmp(entrada->d_name, "..") == 0)
			continue;

		// construimos la ruta completa: "carpeta/nombre"
		size_t longitud =
		    strlen(dirpath) + 1 + strlen(entrada->d_name) + 1;
		char *ruta = malloc(longitud);

		if (ruta == NULL)
			err(EXIT_FAILURE, "Sin memoria");

		snprintf(ruta, longitud, "%s/%s", dirpath, entrada->d_name);

		// lstat nos dice si es carpeta, fichero, enlace...
		if (lstat(ruta, &info) == -1)
			err(EXIT_FAILURE, "No puedo leer info de: %s", ruta);

		if (S_ISDIR(info.st_mode)) {
			// es una subcarpeta, nos metemos dentro
			explorar_carpeta(ruta, outfd);
		} else if (S_ISREG(info.st_mode)) {

			// comprobamos si el nombre termina en ".txt"
			size_t len = strlen(entrada->d_name);
			int es_txt = (len >= 4) &&
			    (strcmp(entrada->d_name + len - 4, ".txt") == 0);

			if (es_txt) {
				int fd = open(ruta, O_RDONLY);

				if (fd == -1)
					err(EXIT_FAILURE, "No puedo abrir: %s",
					    ruta);

				// leemos a trozos para no cargar el fichero entero en memoria
				char trozo[4096];
				ssize_t leidos;

				while ((leidos =
					read(fd, trozo, sizeof trozo)) > 0) {
					// write a veces escribe menos de lo pedido, así que reintentamos
					char *p = trozo;
					ssize_t pendiente = leidos;

					while (pendiente > 0) {
						ssize_t escritos =
						    write(outfd, p, pendiente);
						if (escritos == -1)
							err(EXIT_FAILURE,
							    "Error escribiendo en la salida");
						p += escritos;
						pendiente -= escritos;
					}
				}

				if (leidos == -1)
					err(EXIT_FAILURE, "Error leyendo: %s",
					    ruta);

				close(fd);
			}
		}

		free(ruta);
	}

	closedir(carpeta);
}

int
main(int argc, char *argv[])
{
	argc--;
	argv++;

	if (argc != 1)
		errx(EXIT_FAILURE, "Uso: catrecurtxt <fichero_de_salida>");

	// abrimos el fichero de salida, si ya existe lo vaciamos con O_TRUNC
	int salida = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0666);

	if (salida == -1)
		err(EXIT_FAILURE, "No puedo abrir el fichero de salida: %s",
		    argv[0]);

	// empezamos a explorar desde la carpeta actual
	explorar_carpeta(".", salida);

	close(salida);
	return EXIT_SUCCESS;
}
