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

bool g_swap_endian = false;

void elfCommon(const t_file *fileInfo)
{
	g_swap_endian = false;//(fileInfo->mapping[EI_DATA] == ELFDATA2MSB);

	if (!(fileInfo->mapping[EI_DATA] == ELFDATA2LSB || fileInfo->mapping[EI_DATA] == ELFDATA2MSB)) {
		fprintf(stderr, "woody_woodpacker: '%s': Invalid file endianess\n", fileInfo->name);
		exit(EXIT_FAILURE);
	}

	if (fileInfo->mapping[EI_VERSION] != EV_CURRENT) {
		fprintf(stderr, "woody_woodpacker: '%s': Invalid file version\n", fileInfo->name);
		exit(EXIT_FAILURE);
	}

	if (fileInfo->mapping[EI_CLASS] == ELFCLASS64) {
		elf64(fileInfo);
	}

}