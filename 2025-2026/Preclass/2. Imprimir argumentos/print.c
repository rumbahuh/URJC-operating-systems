#include <stdlib.h>
#include <stdio.h>

void lineas(int num, char *cadena[])
{
	int i = 1;

	for(;i < num; i++) {
		printf("%s\n", cadena[i]);
	}	
	/*
	print.c: In function ‘main’:
	print.c:11:25: warning: format not a string literal and no format arguments [-Wformat-security]
	11 |                         printf(argv[i]);
	   |                         ^~~~~~
	*/ // Hacía falta "%s", en el printf
}

void linea(int num, char *cadena[])
{
	int i = 1;

	for(;i < num; i++) {
		printf("%s", cadena[i]);
		if (i == num - 1) printf("\n");
	}
}

int main(int argc, char *argv[])
{
	lineas(argc, argv);
	linea(argc, argv);

	exit(EXIT_SUCCESS);
}