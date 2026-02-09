#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

int fillList(char *cadena[], int num, int * lista_a_rellenar)
{
	char * endptr;
	long value;

	errno = 0;

	int i = 0;
	for (; i < num; i++) {
		value = strtol(cadena[i], &endptr, 10);

		if (errno != 0 || *endptr != '\0') {
			return 1;
		}

		lista_a_rellenar[i] = (int)value;
	}

	return 0;
}

void getMax(int * lista, int number)
{
	int n = lista[0];
	int i = 1;
	for (; i < number; i++) {
		if (n > lista[i]) {
			n = lista[i];
		}
	}

	printf("El número máximo es: %d\n", n);
}

void getMin(int * lista, int number)
{
	int n = lista[0];
	int i = 1;
	for (; i < number; i++) {
		if (n < lista[i]) {
			n = lista[i];
		}
	}

	printf("El número mínimo es: %d\n", n);
}

int checkpori(int numero)
{
	return numero % 2;
}

void getMed(int * lista, int number)
{
	int med = -1;
	int i = -1;
	if (checkpori(number)) { // es impar
		i = ((number-1)/2);
		med = lista[i];
	} else {
		i = (number/2);
		med = (lista[i-1]+lista[i])/2;
	}

	printf("La mediana es: %d\n", med);
}

void sortList(int *lista, int num)
{
    for (int i = 0; i < num - 1; i++) {
        int min = lista[i];
        int min_idx = i;

        for (int j = i + 1; j < num; j++) {
            if (lista[j] < min) {
                min = lista[j];
                min_idx = j;
            }
        }

        if (min_idx != i) {
            int old_value = lista[i];
            lista[i] = min;
            lista[min_idx] = old_value;
        }
    }

    for (int z = 0; z < num; z++) {
        printf("%d | ", lista[z]);
    }
}

int main(int argc, char *argv[])
{
	argc--;
	argv++;
	int *fillableList = malloc(sizeof(int) * argc);

	if (argc < 1) {
		free(fillableList);
		exit(EXIT_FAILURE);
	}

	if (fillList(argv, argc, fillableList)) {
		free(fillableList);
		exit(EXIT_FAILURE);
	}

	getMax(fillableList, argc);
	getMin(fillableList, argc);
	sortList(fillableList, argc);
	getMed(fillableList, argc);
	
	free(fillableList);
	exit(EXIT_SUCCESS);
}