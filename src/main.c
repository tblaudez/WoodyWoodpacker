//
// main.c
// By: Tristan Blaudez <tblaudez@student.codam.nl>
// Created: 25/10/2021 15:11:46
//

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "woody.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: ./woody_woodpacker <PROG> <ARG>\n");
		return 1;
	}

	t_file fileInfo = {.name = argv[1]};
	int fd;

	// Open source file
	if ((fd = open(fileInfo.name, O_RDONLY)) == -1) {
		perror("open");
		return 1;
	}

	// Get file size by seeking to the end
	if ((fileInfo.size = lseek(fd, 0, SEEK_END)) == -1 || lseek(fd, 0, SEEK_SET) == -1) {
		perror("lseek");
		return 1;
	}

	// Map file to memory
	if ((fileInfo.mapping = mmap(NULL, fileInfo.size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		perror("mmap");
		return 1;
	}

	// Close file
	if (close(fd) == -1) {
		perror("close");
		return 1;
	}

	elfCommon(&fileInfo);

	// Create and open destination file
	if ((fd = open("woody", O_WRONLY | O_CREAT | O_TRUNC, 0744)) == -1) {
		perror("open");
		return 1;
	}

	// Write encrypted data to new file
	if (write(fd, fileInfo.mapping, fileInfo.size) == -1) {
		perror("write");
		return 1;
	}

	// Close file
	if (close(fd) == -1) {
		perror("close");
		return 1;
	}

	if (munmap((void *)fileInfo.mapping, fileInfo.size) == -1) {
		perror("munmap");
		return 1;
	}
}