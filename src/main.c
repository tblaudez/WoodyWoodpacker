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

static t_file *getFileInfo(const char *fileName) {
	static t_file fileInfo;
	int fd;

	fileInfo.name = fileName;
	if ((fd = open(fileInfo.name, O_RDONLY)) == -1) {
		perror("[-] open");
		exit(EXIT_FAILURE);
	}
	printf("[+] File name : '%s'\n", fileInfo.name);

	// Get file size
	if ((fileInfo.size = lseek(fd, 0, SEEK_END)) == -1 || lseek(fd, 0, SEEK_SET) == -1) {
		perror("[-] lseek");
		exit(EXIT_FAILURE);
	}
	printf("[+] File size : %lu bytes\n", fileInfo.size);

	// Map file to memory
	fputs("[+] Mapping file to memory\n", stdout);
	if ((fileInfo.mapping = mmap(NULL, (size_t)fileInfo.size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		perror("[-] mmap");
		exit(EXIT_FAILURE);
	}

	// Close file
	if (close(fd) == -1) {
		perror("[-] close");
		exit(EXIT_FAILURE);
	}

	return &fileInfo;
}

static void createWoody(const t_file *fileInfo) {
	int fd;

	fputs("[+] Writing modified binary to 'woody'\n", stdout);
	// Create and open destination file
	if ((fd = open("woody", O_WRONLY | O_CREAT | O_TRUNC, 0744)) == -1) {
		perror("[-] open");
		exit(EXIT_FAILURE);
	}

	// Write encrypted data to new file
	if (write(fd, fileInfo->mapping, (size_t)fileInfo->size) == -1) {
		perror("[-] write");
		exit(EXIT_FAILURE);
	}

	// Close file
	if (close(fd) == -1) {
		perror("[-] close");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		fputs("usage: ./woody_woodpacker <PROG>\n", stderr);
		exit(EXIT_FAILURE);
	}

	fputs("+++++ Woody Woodpacker +++++\n\n", stdout);

	const t_file *fileInfo = getFileInfo(argv[1]);

	elfCommon(fileInfo);
	createWoody(fileInfo);

	if (munmap((void *)fileInfo->mapping, (size_t)fileInfo->size) == -1) {
		perror("[-] munmap");
		exit(EXIT_FAILURE);
	}

	fputs("\n+++++ Program successfuly packed +++++\n", stdout);
	return 0;
}