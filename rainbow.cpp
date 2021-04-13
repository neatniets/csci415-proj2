#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

#include <crypt.h> // TODO: remove this after anuska implements md5 crypt

#include "rainbow.hpp"

/** The "maximum" number of characters a password would likely be.
 * Useful as a modulus for the length of passwords reduced from a hash.
 * IT IS ASSUMED TO BE <= THE LENGTH OF THE HASH. */
#define PASS_MAX	13
/** The known length of a hash. */
#define HASH_LEN	(128 / CHAR_BIT)
static_assert(PASS_MAX <= HASH_LEN, "PASS_MAX must be <= HASH_LEN");

/* definitions of header declarations */
const char charset[]
	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz123456790";
const size_t charset_len = sizeof(charset) / sizeof(*charset);

/** Salt a password.
 * @post	returned string must be freed
 * @return	salted password string */
static char *
add_salt(
	const char *salt, //!<[in] salt string
	const char *passwd //!<[in] password string
);

char *
hash_chain(
	uchar_t len,
	const char *salt,
	const char *passwd
) {
	/* TODO: finish this */
	fputs("error: hash_chain not implemented\n", stderr);
	exit(1);
}

/* TODO: replace this with anuska's md5 crypt implementation */
char *
hash(
	const char *salt,
	const char *passwd
) {
	/* use md5 crypt library routine */
	const char prefix[] = "$1$";
	const size_t slen = strlen(prefix) + strlen(salt) + 1;
	char *setting = (char *)malloc((slen + 1) * sizeof(*setting));
	memcpy(setting, prefix, strlen(prefix) + 1);
	strcat(setting, salt);
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
	size_t rlen = (hash[key % HASH_LEN] % PASS_MAX) + 1;
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
			rpass[j] ^= hash[i];
		}
	}
	for (size_t i = 0; i < rlen; i++) {
		/* turn bytes into printable pass chars */
		rpass[i] = (unsigned)rpass[i] % charset_len;
	}

	return rpass;
}

static char *
add_salt(
	const char *salt,
	const char *passwd
) {
	/* get lengths */
	size_t slen = strlen(salt);
	size_t plen = strlen(passwd);
	size_t len = slen + plen;
	/* allocate string */
	char *spass = (char *)malloc((len + 1) * sizeof(*spass));

	/* salt password via concatenation */
	memcpy(spass, salt, slen);
	memcpy(spass + slen, passwd, plen);
	spass[len] = '\0';
	return spass;
}
