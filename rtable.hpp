#ifndef TABLE_HPP
#define TABLE_HPP

#include <stddef.h>

/** Build a rainbow table.
 * The password dictionary contains passwords on a single line separated by a
 * single newline.
 * Each line of the rainbow table will be written in the following format:
 * "%zu %s %zu\n", salt, endpoint, offset
 * The endpoint is the last password in that hash chain,
 * and the offset is the file offset of the startpoint password in the
 * dictionary file.
 * The rainbow table will be sorted first on the salt, then on the endpoint.
 * @return	0 on success */
int
build_rbtable(
	const char *dpath, //!<[in] path to password dictionary
	const char *tpath, //!<[in] optional path to rainbow table
	size_t salt_max //!<[in] maximum salt value to create table for
);

#endif
