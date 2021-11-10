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

#define PAYLOAD_OFFSET_IN_FILE (loadSegment->p_offset + loadSegment->p_filesz)
#define PAYLOAD_ADDRESS ((void*)ehdr + PAYLOAD_OFFSET_IN_FILE)
#define PAYLOAD_ADDRESS_END (PAYLOAD_ADDRESS + payloadSize)

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
		if (phdr[i].p_type == PT_LOAD && phdr[i + 1].p_type == PT_LOAD && phdr[i].p_flags & PF_X)
			return &phdr[i];
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
	checkFileCorruption((Elf64_Ehdr *) fileInfo->mapping, fileInfo);

	Elf64_Ehdr *ehdr = (Elf64_Ehdr *) fileInfo->mapping;
	Elf64_Shdr *shdr = (void *) ehdr + ehdr->e_shoff;
	Elf64_Phdr *phdr = (void *) ehdr + ehdr->e_phoff;

	u_char *encryptionKey = (u_char *)generateEncryptionKey();
	Elf64_Phdr *loadSegment = findUsableSegment(phdr, ehdr->e_phnum);
	Elf64_Shdr *textSection = findTextSection(shdr, ehdr->e_shnum, (void *) ehdr + shdr[ehdr->e_shstrndx].sh_offset);

	// Copy payload in code cave
	memcpy(PAYLOAD_ADDRESS, (void *) payload, payloadSize);

	// Edit Mprotect hard-coded variable
	int32_t textSectionAlignedAddress = textSection->sh_addr & ~(4096-1);
	int32_t textSectionRelativeToMprotect = textSectionAlignedAddress - (PAYLOAD_OFFSET_IN_FILE + MPROTECT_DATA_OFFSET);
	int32_t textSectionAlignedSize = ALIGN(textSection->sh_size + (textSection->sh_addr - textSectionAlignedAddress), 4096);

	memcpy(PAYLOAD_ADDRESS + MPROTECT_DATA_OFFSET - 4, &textSectionRelativeToMprotect, 4);
	memcpy(PAYLOAD_ADDRESS + MPROTECT_DATA_SIZE_OFFSET - 4, &textSectionAlignedSize, 4);

	// Edit RC4 hard-coded variable
	uint32_t keySize = KEY_SIZE;
	int32_t textSectionRelativeToRC4 = textSection->sh_addr - (PAYLOAD_OFFSET_IN_FILE + RC4_DATA_OFFSET);
	memcpy(PAYLOAD_ADDRESS + RC4_DATA_OFFSET - 4, &textSectionRelativeToRC4, 4);
	memcpy(PAYLOAD_ADDRESS + RC4_DATA_SIZE_OFFSET - 4, &textSection->sh_size, 4);
	memcpy(PAYLOAD_ADDRESS + RC4_KEY_SIZE_OFFSET - 4, &keySize, 4);

	// Edit jump hard-coded variable
	int32_t oldEntryRelativeToJump = ehdr->e_entry - (PAYLOAD_OFFSET_IN_FILE + PAYLOAD_JUMP_OFFSET);
	memcpy(PAYLOAD_ADDRESS + PAYLOAD_JUMP_OFFSET - 4, &oldEntryRelativeToJump, 4);

	// Fill data with encryption key
	memcpy(PAYLOAD_ADDRESS_END - 0x100, encryptionKey, KEY_SIZE);

	// Set entry to "....WOODY...." print
	ehdr->e_entry = PAYLOAD_OFFSET_IN_FILE + PAYLOAD_WOODY_OFFSET;
	printf("New file entry : %#lX\n", ehdr->e_entry);

	// Encrypt .text section
	RC4((void *)ehdr + textSection->sh_addr, textSection->sh_size, encryptionKey, KEY_SIZE);
}