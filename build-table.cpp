#include <stdio.h>

/** Print usage help. */
static void
help(const char *name); //!<[in] program name

int main(
	int argc,
	char **argv
) {
	help(argv[0]);
	return 0;
}

static void
help(const char *name) {
	printf("Usage:\n"
		"\t%s [-o output_file] input_file\n"
		"\t%s -h\n",
		name, name);
}
