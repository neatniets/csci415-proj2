#define _XOPEN_SOURCE	600

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "rainbow.hpp"
#include "rbtable.hpp"

/** Generate a filename for the rainbow table.
 * @post	the character array points to a staticly allocated location
 * @return	filename string */
static char *
gen_fname(int chainlen); //!<[in] hash chain length

/** Create hash chains for each password + salt & write them to rainbow table.
 * @pre		dictionary file must be open for reading
 * @pre		table file must be open for writing
 * @pre		chain length must be >0
 * @post	position of r/w pointers is undefined
 * @return	0 on success */
static int
chain_passes(
	FILE *dfp, //!<[in] dictionary file ptr
	FILE *tfp, //!<[in] rainbow table file ptr
	int chainlen, //!<[in] hash chain length
	size_t salt_max //!<[in] maximum salt value to account for
);

/** Checks for end-of-file.
 * The eof flag checked by `feof()` only gets set after a failed read for a
 * character at the end of the file.
 * `feof()`, therefore, doesn't let you check for end-of-file BEFORE you read
 * the next character.
 * This function checks for end-of-file without needing to attempt another read
 * and maintains the stream state if it is not end-of-file.
 * @return	true if end-of-file */
static bool
is_eof(FILE *fp); //!<[in] file ptr

/** Sort the rainbow table.
 * @post	contents of file will be altered by sort
 * @return	0 on success */
static int
sort_table(const char *tfile); //!<[in] table filename

int
build_rbtable(
	const char *dfile,
	const char *tfile,
	int chainlen,
	size_t salt_max
) {
	/* open the dictionary file */
	FILE *dfp = fopen(dfile, "r");
	if (dfp == NULL) { // failed to open
		return -1;
	}

	/* generate a filename for table if one not given */
	if (tfile == NULL) {
		tfile = gen_fname(chainlen);
	}
	/* create table file */
	FILE *tfp = fopen(tfile, "w");
	if (tfp == NULL) { // failed to create
		fclose(dfp);
		return -1;
	}

	/* create hash chains */
	int ret = chain_passes(dfp, tfp, chainlen, salt_max);
	/* file ptrs can be closed; no longer needed */
	fclose(dfp);
	fclose(tfp);
	if (ret < 0) { // error
		return ret;
	}

	/* sort the table */
	ret = sort_table(tfile);
	if (ret < 0) { // error
		return ret;
	}

	return 0;
}

static char *
gen_fname(int chainlen) {
	static char fname[128];
	sprintf(fname, "rainbow-table-%d.txt", chainlen);
	return fname;
}

static int
chain_passes(
	FILE *dfp,
	FILE *tfp,
	int chainlen,
	size_t salt_max
) {
	/* init dictionary */
	fseek(dfp, 0, SEEK_SET);
	/* init string */
	char *spass = NULL;
	size_t spsz = 0;
	/* create hash chains for each password + salt */
	while (!is_eof(dfp)) {
		/* get the offset of password */
		off_t doff = ftello(dfp);
		/* get the password */
		size_t splen;
		splen = getline(&spass, &spsz, dfp);
		if (spass[splen-1] == '\n') {
			spass[splen-1] = '\0';
			splen--;
		}

		/* create hash chain for each salt */
		for (size_t i = 0; i < salt_max; i++) {
			/* get endpoint pass */
			char *epass = hash_chain(chainlen, i, spass);
			/* write to table file */
			fprintf(tfp, "%zu %s %jd\n", i, epass, doff);
			/* free pass */
			free(epass);
		}
	}

	/* free remaining memory */
	free(spass);
	return 0;
}

static bool
is_eof(FILE *fp) {
	ungetc(fgetc(fp), fp);
	return feof(fp);
}

static int
sort_table(const char *tfile) {
	/* create child process to sort */
	pid_t pid = fork();
	if (pid == 0) { // child
		const char cmd[] = "sort";
		/* set collation locale to C so the sorting behaves as strcmp()
		 * expects for non-numeric strings */
		setenv("LC_COLLATE", "C", 1);
		/* sort table in-place */
		execlp(cmd, cmd, "-o", tfile,
			"-k", "1n", "-k", "2d", "-k", "3n",
			tfile, NULL);
		/* shouldn't be here */
		perror("failed to exec in child");
		exit(1);
	} else if (pid > 0) { // parent
		/* wait for child to return */
		int status;
		wait(&status);
		/* indicate error if status is not 0 */
		int ret;
		if (status != 0) {
			ret = -1;
		} else {
			ret = 0;
		}
		return ret;
	} else { // error
		perror("failed to fork child");
		return -1;
	}
}
