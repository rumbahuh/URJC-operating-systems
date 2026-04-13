int
main(int argc, char* argv[])
{
	argc--;
	argv++;

	if (argc > 1) {
		fprintf(stderr, "error, invalid arguments\n");
	}
	
	return 0;
}