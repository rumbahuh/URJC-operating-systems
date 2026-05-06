Stack *newstack(int sz);
int isempty(Stack *s);
void push(Stack *s, void *elem);
void *pop (Stack *s);
int nelems(Stack *s);
void dumpstack(Stack *s);
void freestack(Stack *s);