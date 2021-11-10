//
// elf64.c
// By: Tristan Blaudez <tblaudez@student.codam.nl>
// Created: 28/10/2021 12:51:29
//

#include <elf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "woody.h"
#include "libft.h"

extern bool gSwapEndian;

static void checkFileCorruption(const Elf64_Ehdr *ehdr, const t_file *fileInfo) {

	fputs("[+] Checking for file corruption", stdout);
	// Offset too big or too little
	if (SWAP64(ehdr->e_shoff) >= (fileInfo->size - sizeof(Elf64_Shdr)) || SWAP64(ehdr->e_shoff) < (sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr))) {
		fputs("\n[-] Invalid section header table offset\n", stderr);
		exit(EXIT_FAILURE);
	}

	// No sections
	if (SWAP16(ehdr->e_shnum) == 0) {
		fputs("\n[-] Invalid section header table entries\n", stderr);
		exit(EXIT_FAILURE);
	}

	// Invalid type
	if (!(SWAP16(ehdr->e_type) >= 1 && SWAP16(ehdr->e_type) <= 4)) {
		fputs("\n[-] Invalid file type\n", stderr);
		exit(EXIT_FAILURE);
	}
	fputs(" => OK\n", stdout);
}

static Elf64_Phdr *findUsableSegment(Elf64_Ehdr *ehdr) {
	Elf64_Phdr *phdr = (void *)ehdr + SWAP64(ehdr->e_phoff);

	fputs("[+] Looking for code cave", stdout);
	for (int i = 0; i < SWAP16(ehdr->e_phnum) - 1; i++) {
		if (SWAP32(phdr[i].p_type) == PT_LOAD && SWAP32(phdr[i + 1].p_type) == PT_LOAD && SWAP32(phdr[i].p_flags & PF_X)) {
			uint64_t codeCaveOffset = SWAP64(phdr[i].p_offset) + SWAP64(phdr[i].p_filesz);
			uint64_t codeCaveSize = SWAP64(phdr[i+1].p_offset) - (codeCaveOffset);
			if (codeCaveSize > payloadSize) {
				printf(" => FOUND (offset : %#lX / size : %ld bytes)\n", codeCaveOffset, codeCaveSize);
				return &phdr[i];
			}
		}
	}
	fputs("\n[-] Could not find suitable code cave\n", stderr);
	exit(EXIT_FAILURE);
}

static const Elf64_Shdr *findTextSection(Elf64_Ehdr *ehdr) {
	const Elf64_Shdr *shdr = (void *)ehdr + SWAP64(ehdr->e_shoff);
	const char *shdrstrtab = (void *) ehdr + SWAP64(shdr[SWAP16(ehdr->e_shstrndx)].sh_offset);

	fputs("[+] Looking for program text section", stdout);
	for (size_t i = 0; i < SWAP16(ehdr->e_shnum); i++) {
		if (!ft_strcmp(shdrstrtab + SWAP32(shdr[i].sh_name), ".text")) {
			fputs(" => OK\n", stdout);
			return &shdr[i];
		}
	}
	fputs("\n[-] Could not find program text section\n", stderr);
	exit(EXIT_FAILURE);
}

static void injectPayloadAtOffset(size_t offset, const Elf64_Ehdr *ehdr, const Elf64_Shdr *textSection, const u_char *encryptionKey) {

	// Copy payload in code cave
	printf("[+] Injecting payload at offset %#lX\n", offset);
	ft_memcpy((void*)ehdr + offset, (void *) payload, payloadSize);

	// Edit Mprotect hard-coded variable
	int32_t textSectionAlignedAddress = SWAP64(textSection->sh_addr) & ~(4096-1);
	int32_t textSectionRelativeToMprotect = SWAP32(textSectionAlignedAddress - (offset + MPROTECT_DATA_OFFSET));
	int32_t textSectionAlignedSize = SWAP32(ALIGN(SWAP64(textSection->sh_size) + (SWAP64(textSection->sh_addr) - textSectionAlignedAddress), 4096));
	fputs("[+] Setting up mprotect() \n", stdout);
	ft_memcpy((void*)ehdr + offset + MPROTECT_DATA_OFFSET - 4, &textSectionRelativeToMprotect, 4);
	ft_memcpy((void*)ehdr + offset + MPROTECT_DATA_SIZE_OFFSET - 4, &textSectionAlignedSize, 4);

	// Edit RC4 hard-coded variable
	uint32_t keySize = SWAP32(KEY_SIZE);
	int32_t textSectionRelativeToRC4 = SWAP32(SWAP64(textSection->sh_addr) - (offset + RC4_DATA_OFFSET));
	uint32_t textSectionSize = SWAP32(SWAP64(textSection->sh_size));
	fputs("[+] Setting up RC4 decryptor\n", stdout);
	ft_memcpy((void*)ehdr + offset + RC4_DATA_OFFSET - 4, &textSectionRelativeToRC4, 4);
	ft_memcpy((void*)ehdr + offset + RC4_DATA_SIZE_OFFSET - 4, &textSectionSize, 4);
	ft_memcpy((void*)ehdr + offset + RC4_KEY_SIZE_OFFSET - 4, &keySize, 4);

	// Edit jump hard-coded variable
	int32_t oldEntryRelativeToJump = SWAP32(SWAP64(ehdr->e_entry) - (offset + PAYLOAD_JUMP_OFFSET));
	printf("[+] Setting up jump to old program entry => %#lX\n", ehdr->e_entry);
	ft_memcpy((void*)ehdr + offset + PAYLOAD_JUMP_OFFSET - 4, &oldEntryRelativeToJump, 4);

	// Fill data with encryption key
	fputs("[+] Filling payload with encryption key\n", stdout);
	ft_memcpy((void*)ehdr + offset + payloadSize - 0x100, encryptionKey, KEY_SIZE);

}

void elf64(const t_file *fileInfo) {
	checkFileCorruption((Elf64_Ehdr *) fileInfo->mapping, fileInfo);

	Elf64_Ehdr *ehdr = (Elf64_Ehdr *) fileInfo->mapping;
	const Elf64_Shdr *textSection = findTextSection(ehdr);
	const Elf64_Phdr *segment = findUsableSegment(ehdr);
	const u_char *encryptionKey = generateEncryptionKey();

	injectPayloadAtOffset(SWAP64(segment->p_offset) + SWAP64(segment->p_filesz), ehdr, textSection, encryptionKey);

	// Set entry to "....WOODY...." print
	ehdr->e_entry = segment->p_offset + segment->p_filesz + PAYLOAD_WOODY_OFFSET;
	printf("[+] Setting new program entry => %#lX\n", ehdr->e_entry);

	// Encrypt .text section
	fputs("[+] Encrypting program text section\n", stdout);
	RC4((void *)ehdr + textSection->sh_addr, textSection->sh_size, encryptionKey, KEY_SIZE);
}