#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Crea una pila vacía con un array del tamaño dado.
 * El tamaño debe ser mayor que cero. 
 * En caso de error, devolverá NULL.
 */
Stack
*newstack(int sz)
{
	Stack *stack = malloc(sizeof(Stack));
	if (stack == NULL) {
		fprintf(stderr, "error, failed to malloc stack\n");
		return NULL;
	}
	stack->size = sz;
	stack->elems = 0;
	stack->elemento = malloc(sizeof(void *) * sz);
	if (stack->elemento == NULL) {
		fprintf(stderr, "error, failed to malloc stack array\n");
		free(stack);
		return NULL;
	}
	pthread_mutex_init(&stack->lock, NULL);

	return stack;
}

/*
 * Retorna true si la pila está vacía.
 */
int
isempty(Stack *s)
{
	
	return s->elems == 0;
}

/*
 * Mete en la pila un elemento.
 * Si la capacidad de la pila se ha alcanzado,
 * se tiene que hacer un realloc para doblar su tamaño.
 */
void
push(Stack *s, void *elem)
{
	void * tmp;
	pthread_mutex_lock(&s->lock);

	if (s->elems + 1 > s->size) {
		tmp = realloc(s->elemento, sizeof(void*) * s->size * 2);
		if (tmp == NULL) {
			fprintf(stderr, "error doubling stack array\n");
			pthread_mutex_unlock(&s->lock);
			return;
		}
		s->elemento = tmp;
		s->size = s->size * 2;
	}
	s->elemento[s->elems] = elem;
	s->elems++;
	
	pthread_mutex_unlock(&s->lock);
}

/*
 * Devuelve el elemento. Si la pila está vacía,
 * pop devuelve NULL. No es necesario reducir
 * el tamaño del array en ningún caso.
 */
void *
pop (Stack *s)
{	
	pthread_mutex_lock(&s->lock);

	if (s->elems == 0) {
		pthread_mutex_unlock(&s->lock);
		return NULL;
	}
	s->elems--;

	pthread_mutex_unlock(&s->lock);
	return s->elemento[s->elems];
}

/*
 * Ver cuántos elementos hay en la pila.
 */
int
nelems(Stack *s)
{
	return 0;
}

/* 
 * Imprime la pila por la salida de errores en un
 * formato legible, con su capacidad, el número
 * de  elementos almacenados y los elementos 
 * (imprimiendo los punteros).
 */
void
dumpstack(Stack *s)
{
}

/* Libera toda la memoria de la pila.
 * No se puede llamar a esta función
 * mientras que otros hilos están
 * usando la pila. Esto es responsabilidad
 * del usuario de la pila. Esta función
 * solo debe liberar los recursos de la pila,
 * sin afectar a los elementos almacenados. 
 */
void
freestack(Stack *s)
{
}