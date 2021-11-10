//
// elf_common.c
// By: Tristan Blaudez <tblaudez@student.codam.nl>
// Created: 28/10/2021 12:21:58
//

#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <stdbool.h>
#include "woody.h"
#include "libft.h"

bool gSwapEndian = false;

void elfCommon(const t_file *fileInfo)
{
	gSwapEndian = (fileInfo->mapping[EI_DATA] == ELFDATA2MSB);

	fputs("[+] Checking file type", stdout);
	if (ft_strncmp((const char*)fileInfo->mapping, ELFMAG, SELFMAG) != 0) {
		fputs("\n[-] Invalid file format. Only ELF is supported\n", stderr);
		exit(EXIT_FAILURE);
	}

	if (!(fileInfo->mapping[EI_DATA] == ELFDATA2LSB || fileInfo->mapping[EI_DATA] == ELFDATA2MSB)) {
		fputs("\n[-] Invalid file endianess. File might be corrupted\n", stderr);
		exit(EXIT_FAILURE);
	}

	if (fileInfo->mapping[EI_CLASS] == ELFCLASS64) {
		fputs(" => ELF64 detected\n", stdout);
		elf64(fileInfo);
	} else if (fileInfo->mapping[EI_CLASS] == ELFCLASS32) {
		fputs(" => ELF32 detected\n", stdout);
	} else {
		fputs(" => INVALID\n[-] Invalid file class. Only ELF64 and ELF32 supported\n", stderr);
		exit(EXIT_FAILURE);
	}
}