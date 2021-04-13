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
	const salt_t salt = 123;
	const char passwd[] = "gamer";

	puts("test if hashing is deterministic");
	char *det[2];
	for (int i = 0; i < 2; i++) {
		puts("hashing");
		det[i] = hash(salt, passwd);
		printf("%d: %s\n", i, det[i]);
	}
	assert(strcmp(det[0], det[1]) == 0);
	puts("hashes are equal.\n");

	puts("testing if reduction is deterministic");
	char *pdet[2];
	for (int i = 0; i < 2; i++) {
		puts("reducing");
		pdet[i] = reduce(0, det[i]);
		printf("%d: %s\n", i, pdet[i]);
	}
	assert(strcmp(pdet[0], pdet[1]) == 0);
	puts("passes are equal.\n");

	for (int i = 0; i < 2; i++) {
		puts("freeing hash");
		free_hash(det[i]);
		puts("freeing pass");
		free(pdet[i]);
	}

	puts("\ncomparing different reduction key values");
	char *hash1 = hash(salt, passwd);
	printf("hash = %s\n", hash1);
	char *pass;
	for (int i = 0; i <= 10; i++) {
		puts("reducing");
		pass = reduce(i, hash1);
		printf("%d: %s\n", i, pass);
		puts("freeing pass");
		free(pass);
	}

	puts("freeing hash");
	free_hash(hash1);

	return 0;
}
