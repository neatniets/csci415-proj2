#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "rainbow.hpp"

int
main(
	int argc,
	char **argv
) {
	const char salt[] = "as";
	const char passwd[] = "gamer";

	puts("test if hashing is deterministic");
	char *det[2];
	for (int i = 0; i < 2; i++) {
		puts("hashing");
		det[i] = hash(salt, passwd);
		printf("%d: %s\n", i, det[i]);
	}
	assert(strcmp(det[0], det[1]) == 0);
	puts("hashes are equal.");

	return 0;
}
