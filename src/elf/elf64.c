//
// elf64.c
// By: Tristan Blaudez <tblaudez@student.codam.nl>
// Created: 28/10/2021 12:51:29
//

#include <elf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "woody.h"

#define PAYLOAD_START ((void*)ehdr + ehdr->e_entry)
extern bool g_swap_endian;

static void checkFileCorruption(const Elf64_Ehdr *ehdr, const t_file *fileInfo) {
	// Offset too big or too little
	if (SWAP64(ehdr->e_shoff) >= (fileInfo->size - sizeof(Elf64_Shdr))
		|| SWAP64(ehdr->e_shoff) < (sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr))) {
		fprintf(stderr, "woody_woodpacker: '%s': Invalid section header table offset\n", fileInfo->name);
		exit(EXIT_FAILURE);
	}

	// No sections
	if (SWAP16(ehdr->e_shnum) == 0) {
		fprintf(stderr, "woody_woodpacker: '%s': Invalid section header table entry count\n", fileInfo->name);
		exit(EXIT_FAILURE);
	}

	// Invalid type
	if (!(SWAP16(ehdr->e_type) >= 1 && SWAP16(ehdr->e_type) <= 4)) {
		fprintf(stderr, "woody_woodpacker: '%s': Invalid file type\n", fileInfo->name);
		exit(EXIT_FAILURE);
	}
}

static Elf64_Phdr *findUsableSegment(Elf64_Phdr *phdr, size_t size) {
	for (size_t i = 0; i < size - 1; i++) {
		if (phdr[i].p_type == PT_LOAD && phdr[i + 1].p_type == PT_LOAD && phdr[i].p_flags & PF_X) {
			printf("Cave Code size : %lu\n", phdr[i+1].p_offset - (phdr[i].p_offset + phdr[i].p_filesz));
			return &phdr[i];
		}
	}
	fputs("Could not find suitable code cave\n", stderr);
	exit(EXIT_FAILURE);
}

static Elf64_Shdr *findTextSection(Elf64_Shdr *shdr, size_t size, const char *shdrstrtab) {
	for (size_t i = 0; i < size; i++) {
		if (strcmp(shdrstrtab + shdr[i].sh_name, ".text") == 0)
			return &shdr[i];
	}
	fputs("Could not find .text section\n", stderr);
	exit(EXIT_FAILURE);
}

void elf64(const t_file *fileInfo) {
	checkFileCorruption((Elf64_Ehdr *)fileInfo->mapping, fileInfo);

	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)fileInfo->mapping;
	Elf64_Shdr *shdr = (void *)ehdr + ehdr->e_shoff;
	Elf64_Phdr *phdr = (void *)ehdr + ehdr->e_phoff;

	Elf64_Phdr *loadSegment = findUsableSegment(phdr, ehdr->e_phnum);
	Elf64_Shdr *textSection = findTextSection(shdr, ehdr->e_shnum, (void *)ehdr + shdr[ehdr->e_shstrndx].sh_offset);
	int32_t oldEntry = ehdr->e_entry;

	/*// Set new program entry
	ehdr->e_entry = loadSegment->p_offset + loadSegment->p_filesz;
	printf("Old entry : %#X\n", oldEntry);
	printf("New entry : %#lX\n\n", ehdr->e_entry);

	// Copy payload in code cave
	memcpy(PAYLOAD_START, (void*)payload, payloadSize);

	uint64_t data = textSection->sh_offset;
	memcpy(PAYLOAD_START + 41, &data, sizeof(data));
	displayHexData("Data : ", PAYLOAD_START + 39, 10);

	uint64_t dataSize = textSection->sh_size;
	memcpy(PAYLOAD_START + 51, &dataSize, sizeof(dataSize));
	displayHexData("Data size : ", PAYLOAD_START + 49, 10);

	uint64_t keySize = KEY_SIZE;
	memcpy(PAYLOAD_START + 68, &keySize, sizeof(keySize));
	displayHexData("Key size : ", PAYLOAD_START + 66, 10);

	uint64_t buffer = textSection->sh_offset;
	memcpy(PAYLOAD_START + 78, &buffer, sizeof(buffer));
	displayHexData("Buffer : ", PAYLOAD_START + 76, 10);

	int32_t jump = oldEntry - (ehdr->e_entry + 269);
	memcpy(PAYLOAD_START + 265, &jump, sizeof(jump));
	displayHexData("Jump : ", PAYLOAD_START + 264, 5);

	memcpy(PAYLOAD_START + 284, encryptionKey, KEY_SIZE);
	displayHexData("Key : ", PAYLOAD_START + 284, KEY_SIZE);

	// Enlarge segment to contain payload
	loadSegment->p_filesz += payloadSize;
	loadSegment->p_memsz += payloadSize;*/

	u_char *encryptionKey = generateEncryptionKey();
	// Encrypt .text section
	displayHexData("Before : ", (void*)ehdr + textSection->sh_offset, 32);
	RC4((void*)ehdr + textSection->sh_offset, textSection->sh_size, encryptionKey, KEY_SIZE, (void*)ehdr + textSection->sh_offset);
	displayHexData("After : ", (void*)ehdr + textSection->sh_offset, 32);
	RC4((void*)ehdr + textSection->sh_offset, textSection->sh_size, encryptionKey, KEY_SIZE, (void*)ehdr + textSection->sh_offset);
	displayHexData("After After : ", (void*)ehdr + textSection->sh_offset, 32);
}