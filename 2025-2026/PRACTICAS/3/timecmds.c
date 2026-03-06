#include <stdio.h>

int
tokenize(int *num, char * chain[])
{
	int localstatus = 0;
	(void)chain;
	*num = 2;

	return localstatus;
}

int
main(int argc, char * argv[])
{
	int status = 0, i = 0;
	argc--;
	argv++;

	status = tokenize(&argc, argv);
	
	// We check new argv structure
	for(; i < argc; i++) {
		fprintf(stdout, "%s\n", argv[i]);
	}
	
	return status;
}