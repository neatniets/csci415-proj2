#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include <crypt.h> // TODO: remove this after anuska implements md5 crypt

#include "rainbow.hpp"

/** The "maximum" number of characters a password would likely be.
 * Useful as a modulus for the length of passwords reduced from a hash.
 * IT IS ASSUMED TO BE <= THE LENGTH OF THE HASH. */
#define PASS_MAX	13
/** The "minimum" number of characters a password would likely be.
 * Must be < maximum. */
#define PASS_MIN	5
static_assert(PASS_MIN < PASS_MAX, "PASS_MIN must be < PASS_MAX");
/** The known length of a hash. */
#define HASH_LEN	(128 / CHAR_BIT)
static_assert(PASS_MAX <= HASH_LEN, "PASS_MAX must be <= HASH_LEN");
/** The length of the string a salt is converted into. */
#define SALT_CH_LEN	8
/** Mask for a single byte. */
#define BYTE_MASK	0xFF

/* definitions of header declarations */
const char charset[]
	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
const size_t charset_len = sizeof(charset) / sizeof(*charset) - 1;

/** Turn a salt into a string to be used in hashing. */
static void
salt2str(
	char saltstr[SALT_CH_LEN + 1], //!<[out] salt string
	salt_t salt //!<[in] numeric salt
);

char *
hash_chain(
	uchar_t len,
	salt_t salt,
	const char *passwd
) {
	/* init vals */
	char *h = hash(salt, passwd);
	char *p = reduce(0, h);
	free_hash(h);
	h = NULL;

	/* produce hash chain */
	for (int i = 1; i < len; i++) {
		/* hash password */
		h = hash(salt, p);
		free(p);
		/* reduce hash */
		p = reduce(i, h);
		free_hash(h);
	}

	/* return endpoint */
	return p;
}

/* TODO: replace this with anuska's md5 crypt implementation */
char *
hash(
	salt_t salt,
	const char *passwd
) {
	char ssalt[SALT_CH_LEN + 1];
	salt2str(ssalt, salt);

	/* use md5 crypt library routine */
	const char prefix[] = "$1$";
	const size_t slen = strlen(prefix) + strlen(ssalt) + 1;
	char *setting = (char *)malloc((slen + 1) * sizeof(*setting));
	memcpy(setting, prefix, strlen(prefix) + 1);
	strcat(setting, ssalt);
	setting[slen - 1] = '$';
	setting[slen] = '\0';

	const char *enc_hash = crypt(passwd, setting);

	/* encoded hash has the 'setting' string at its start, with the hash
	 * following after a separator */
	const char *hash = enc_hash + strlen(setting) + 1;

	/* hash is not dynamically allocated & needs to be copied */
	char *hashcpy = (char *)malloc((HASH_LEN + 1) * sizeof(*hashcpy));
	memcpy(hashcpy, hash, HASH_LEN);
	hashcpy[HASH_LEN] = '\0';

	/* free setting */
	free(setting);

	return hashcpy;
}

void
free_hash(char *hash) {
	free(hash);
}

char *
reduce(
	uchar_t key,
	const char *hash
) {
	/* obtain a reduction pass length */
	size_t rlen = (hash[key % HASH_LEN] % (PASS_MAX + 1 - PASS_MIN))
		+ PASS_MIN;
	/* allocate rpass */
	char *rpass = (char *)malloc((rlen + 1) * sizeof(*rpass));
	memset(rpass, '\0', rlen + 1);

	/* reduce the hash using the key */
	for (size_t i = 0; i < HASH_LEN; i++) {
		/* creating some indices to apply hash char on */
		const size_t inds[] = {
			(i ^ key) % rlen,
			(hash[i] ^ key) % rlen
		};
		const int ninds = sizeof(inds) / sizeof(*inds);

		/* apply hash char to those indices */
		for (int j = 0; j < ninds; j++) {
			rpass[inds[j]] ^= hash[i];
		}
	}
	for (size_t i = 0; i < rlen; i++) {
		/* turn bytes into printable pass chars */
		rpass[i] = charset[(unsigned)rpass[i] % charset_len];
	}

	return rpass;
}

static void
salt2str(
	char saltstr[SALT_CH_LEN + 1],
	salt_t salt
) {
	/* salt has to be in base64 notation, so using the base64 command */

	/* generate pipes to talk to child */
	int pipes2ch[2]; // parent -> child
	int pipes2p[2]; // child -> parent
	pipe(pipes2ch);
	pipe(pipes2p);

	/* create child */
	pid_t pid = fork();
	if (pid == 0) { // child
		/* close unused pipes */
		close(pipes2ch[1]);
		close(pipes2p[0]);
		/* set stdin to come from pipe */
		dup2(pipes2ch[0], fileno(stdin));
		/* set stdout to go to pipe */
		dup2(pipes2p[1], fileno(stdout));
		/* exec base64 */
		const char cmd[] = "base64";
		execlp(cmd, cmd, "-", NULL);
		/* shouldn't be here */
		fputs("failed to exec base64\n", stderr);
		exit(1);
	} else if (pid > 0) { // parent
		/* close unused pipes */
		close(pipes2ch[0]);
		close(pipes2p[1]);
		/* send salt to be encoded */
		write(pipes2ch[1], &salt, sizeof(salt));
		/* kill pipe to signal end of transmission */
		close(pipes2ch[1]);
		/* wait for child to die */
		int status;
		wait(&status);
		if (status != 0) { // child failed
			exit(1);
		} else { // child success
			read(pipes2p[0], saltstr, SALT_CH_LEN);
			close(pipes2p[0]);
			saltstr[SALT_CH_LEN] = '\0';
		}
	} else { // error
		fputs("failed to generate child for base64\n", stderr);
		exit(1);
	}
}
