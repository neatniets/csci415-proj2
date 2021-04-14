#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>

#include "rbtable.hpp"

/** Print usage help. */
static void
help(const char *name); //!<[in] program name

/** Print an error message to stderr. */
static void
printf_err(
	const char *fmt, //!<[in] printf format
	... //!<[in] args to format
);

int main(
	int argc,
	char **argv
) {
	/* set default values */
	const char *outf = NULL;
	const char *inf = NULL;
	int chainlen = 16;
	size_t salt_max = 4096;

	bool is_parsing = true;
	while (is_parsing) {
		/* parse options */
		int ret = getopt(argc, argv, ":o:s:l:h");
		switch (ret) {
		case 'h': // help
			help(argv[0]);
			return 0;
		case 'o': // output filename
			outf = optarg;
			break;
		case 's': // salt
			salt_max = atoll(optarg);
			if (salt_max > 0) { // no error
				break;
			} // error
			printf_err("'%s' is not a valid argument for '-%c'\n",
				optarg, ret);
			return 1;
		case 'l': // chain length
			chainlen = atoll(optarg);
			if (chainlen > 0) { // no error
				break;
			} // error
			printf_err("'%s' is not a valid argument for '-%c'\n",
				optarg, ret);
			return 1;
		case ':': // missing option arg
			printf_err("option '-%c' expects an argument\n",
				optopt);
			return 1;
		case '?': // unknown option
			printf_err("option '-%c' is not recognized\n",
				optopt);
			return 1;
		case -1: // end of options
			if (argv[optind] == NULL) { // missing input file
				printf_err("missing password dictionary"
					" file\n");
				return 1;
			} else {
				inf = argv[optind];
				is_parsing = false;
			}
		}
	}

	if (build_rbtable(inf, outf, chainlen, salt_max) != 0) {
		printf_err("failed to create rainbow table\n");
		return 1;
	}

	return 0;
}

/** Usage format string. */
#define USAGE_FMT \
	"Usage:\n" \
	"\t%s [-o table_file] [-s max_salt] [-l chainlen]\n" \
	"\t\t[--] dict_file\n" \
	"\t%s -h\n"

static void
help(const char *name) {
	printf(USAGE_FMT, name, name);
}

static void
printf_err(
	const char *fmt,
	...
) {
	fputs("error: ", stderr);
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}
