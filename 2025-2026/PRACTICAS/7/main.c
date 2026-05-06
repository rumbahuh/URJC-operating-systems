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
		if (v == NULL) {
			fprintf(stderr, "error casting\n");
			return NULL;
		}
		if (v->id != threaddna->id) {
			fprintf(stdout, "%d", counter);
		}
	}

	return NULL;
}

int main(int argc, char* argv[])
{
	int total = 100;
	Stack *stack = newstack(total);
	pthread_t threads[total];
	Args *args[total];

	int i;

	for (i = 0; i < total; i++) {
		args[i] = malloc(sizeof(Args));
		if (args[i] == NULL) {
			fprintf(stderr, "error malloc for thread\n");
			break;
		}

		args[i]->id = i;
		args[i]->s = stack;

		if (pthread_create(&threads[i], NULL, rellenarstack, args[i]) != 0) {
			fprintf(stderr, "error creating thread %d\n", i);
			free(args[i]);
			break;
		}
	}

	for (int j = 0; j < i; j++) {
		pthread_join(threads[j], NULL);
	}

	for (int j = 0; j < i; j++) {
		free(args[j]);
	}

	return 0;
}