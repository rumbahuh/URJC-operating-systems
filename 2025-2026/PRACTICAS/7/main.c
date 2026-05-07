#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "stack.h"

struct Value {
	int v;
	int id;
};
typedef struct Value Value;

struct Args {
	int id;
	Stack *s;	
};
typedef struct Args Args;

/* 
 * Cada thread inserta 100 elementos y extrae 40.
 * Ademas cuenta cuántos no son suyos.
 */
void*
rellenarstack(void *arg)
{
	int i = 0, j = 0;
	int topop = 40;
	int idmax = 100;
	int counter = 0;

	Args *threaddna = (Args*) arg;

	for(; i < idmax; i++) {
		Value *value = malloc(sizeof(Value));
		if (value == NULL) {
			fprintf(stderr, "error to malloc value\n");
			return NULL;
		}
		value->v = i;
		value->id = threaddna->id;
		push(threaddna->s, value);
	}

	for(; j < topop; j++) {
		void *poppedelem = pop(threaddna->s);
		if (poppedelem == NULL) {
			fprintf(stderr, "error popped element returned NULL\n");
			return NULL;
		}
		Value *v = (Value*) poppedelem;
		if (v->id != threaddna->id) {
			counter++;
		}
		free(v);
	}

	fprintf(stdout, "Thread %d: %d elementos ajenos\n", threaddna->id, counter);

	return NULL;
}

/* 
 * Verifica que los elementos estén en orden decreciente
 * y los imprime.
 */
int
printall(Stack *s, int total)
{
	// Hay 100 ids
	int lastseen[100];
	for (int k = 0; k < 100; k++)
        lastseen[k] = 100;

	for (int i = 0; i < total; i++) {
        Value *v = (Value *)pop(s);
		// Si el valor no es estrictamente decreciente respecto al anterior del mismo id
        if (v->v >= lastseen[v->id]) {
            fprintf(stderr, "error: id %d, valor %d no es decreciente (anterior: %d)\n",
                    v->id, v->v, lastseen[v->id]);
			dumpstack(s);
            free(v);

			// Liberamos los elementos restantes de la pila antes de salir
			void *elem;
            while ((elem = pop(s)) != NULL)
                free(elem);
            return 1;
        }
		// Actualizamos el valor anterior
        lastseen[v->id] = v->v;
        free(v);
    }

	return 0;
}

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	int status = 0;
	// Índices sobre los que iteramos
	int i = 0, j = 0, w = 0;

	int total = 100;

	Stack *stack = newstack(total);
	if (stack == NULL) {
		fprintf(stderr, "error to create stack\n");
		return 1;
	}

	pthread_t *threads = malloc(sizeof(pthread_t) * total);
	if (threads == NULL) {
		fprintf(stderr, "error malloc threads\n");
		freestack(stack);
		return 1;
	}
	Args     **args    = malloc(sizeof(Args *)    * total);
	if (args == NULL) {
		fprintf(stderr, "error malloc args\n");
		freestack(stack);
		free(threads);
		return 1;
	}

	for (; i < total; i++) {
		args[i] = malloc(sizeof(Args));
		if (args[i] == NULL) {
			fprintf(stderr, "error malloc for thread\n");
			status = 1;
			break;
		}

		args[i]->id = i;
		args[i]->s = stack;

		if (pthread_create(&threads[i], NULL, rellenarstack, args[i]) != 0) {
			fprintf(stderr, "error creating thread %d\n", i);
			free(args[i]);
			status = 1;
			break;
		}
	}

	// Esperamos a que los threads acaben
	for (; j < i; j++) {
		pthread_join(threads[j], NULL);
	}

	// Última especificación
	int elementosfinales = nelems(stack);
	if (elementosfinales != (60*100)) {
		fprintf(stderr, "error in nelems in stack\n");
		// Imprimimos el estado de la pila
		dumpstack(stack);
		void *elem;
		while ((elem = pop(stack)) != NULL)
			free(elem);
		status = 1;
	} else {
		status = printall(stack, elementosfinales);
	}

	// Libera hasta i que es donde se paró el bucle
	for (; w < i; w++) {
		free(args[w]);
	}

	free(threads);
	free(args);
	freestack(stack);

	return status;
}