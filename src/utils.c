//
// utils.c
// By: Tristan Blaudez <tblaudez@student.codam.nl>
// Created: 25/10/2021 14:59:37
//

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "woody.h"

void displayHexData(const char *prefix, u_char *data, size_t size) {
	fputs(prefix, stdout);
	for (size_t i = 0; i < size; i++) {
		printf("%02hhX ", data[i]);
	}
	fputc('\n', stdout);
}

/* Fill the key buffer with random bytes from '/dev/urandom' */
u_char *generateEncryptionKey(void) {
	static u_char encryptionBuffer[KEY_SIZE];
	int fd;

	if ((fd = open("/dev/urandom", O_RDONLY)) == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	if (read(fd, encryptionBuffer, KEY_SIZE) == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	close(fd);
	return encryptionBuffer;
}