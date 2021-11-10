//
// woody.h
// By: Tristan Blaudez <tblaudez@student.codam.nl>
// Created: 25/10/2021 15:12:13
//

#pragma once

#include <sys/types.h>

#define KEY_SIZE 8

#define PAYLOAD_WOODY_OFFSET (0xD1)
#define MPROTECT_DATA_OFFSET (0XF5)
#define MPROTECT_DATA_SIZE_OFFSET (0XFA)
#define RC4_DATA_OFFSET (0x108)
#define RC4_DATA_SIZE_OFFSET (0x10D)
#define RC4_KEY_SIZE_OFFSET (0x119)
#define PAYLOAD_JUMP_OFFSET (0x123)


#define ALIGN(address, base) (((address) + (base) - 1) & ~((base) - 1))
#define SWAP16(x) (gSwapEndian ? __builtin_bswap16(x) : x)
#define SWAP32(x) (gSwapEndian ? __builtin_bswap32(x) : x)
#define SWAP64(x) (gSwapEndian ? __builtin_bswap64(x) : x)


typedef struct {
	const char *name;
	const u_char *mapping;
	ssize_t size;
} t_file;

// payload.asm
void payload(void);

extern uint32_t payloadSize;
extern uint32_t dataOffset;
extern uint32_t jumpInstructionOffset;

// RC4.asm
void RC4(u_char *buffer, size_t bufferSize, const u_char *encryptionKey, size_t keySize);

// elf_common.c
void elfCommon(const t_file *fileInfo);

void elf64(const t_file *fileInfo);

// utils.c
u_char *generateEncryptionKey(void);

void displayHexData(const char *prefix, u_char *data, size_t size);