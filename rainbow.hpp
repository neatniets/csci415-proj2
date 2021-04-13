#ifndef RAINBOW_HPP
#define RAINBOW_HPP

#include <stddef.h>

/** Set of characters allowed in user passwords. */
extern const char charset[];
/** Number of characters in the character set. */
extern const size_t charset_len;

typedef unsigned char uchar_t;
typedef long long salt_t;

/** Precompute a hash chain, obtaining the endpoint password.
 * @post	returned password should be freed
 * @return	endpoint password */
char *
hash_chain(
	uchar_t len, //!<[in] hash chain length
	salt_t salt, //!<[in] salt string
	const char *passwd //!<[in] startpoint password string
);

/** Create a hash from a salt and a password.
 * @post        hash must be freed
 * @return      hash string */
char *
hash(
	salt_t salt, //!<[in] salt string
	const char *passwd //!<[in] password string
);

/** Free memory allocated to a hash. */
void
free_hash(char *hash); //!<[in] hash string

/** Reduce a hash to a password string.
 * A different reduction function is used depending on the key.
 * Only 256 reduction functions are supported.
 * @post        password string must be freed
 * @return      password string */
char *
reduce(
	uchar_t key, //!<[in] key for reduction function
	const char *hash //!<[in] hash string
);

#endif /* !RAINBOW_HPP */
