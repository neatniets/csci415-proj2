#ifndef TABLE_HPP
#define TABLE_HPP

#include <stddef.h>

/** Build a rainbow table.
 * The filename for the table may be NULL; a filename will be generated.
 *
 * The password dictionary contains passwords on a single line separated by a
 * single newline.
 *
 * Each line of the rainbow table will be written in the following format:
 * "%zu %s %zu\n", salt, endpoint, offset
 * The endpoint is the last password in that hash chain,
 * and the offset is the file offset of the startpoint password in the
 * dictionary file.
 * The rainbow table will be sorted first on the salt, then on the endpoint.
 * @pre		hash chain length must be >0
 * @return	0 on success */
int
build_rbtable(
	const char *dfile, //!<[in] filename of password dictionary
	const char *tfile, //!<[in] optional filename of rainbow table
	int chainlen, //!<[in] length of hash chains
	size_t salt_max //!<[in] maximum salt value to create table for
);

#endif
