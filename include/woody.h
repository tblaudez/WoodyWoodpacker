//
// woody.h
// By: Tristan Blaudez <tblaudez@student.codam.nl>
// Created: 25/10/2021 15:12:13
//

#pragma once

#include <sys/types.h>

#define KEY_SIZE 8
#define KEY_MAX_SIZE 256
#define WOODY_SIZE 14

#define ALIGN(address, base) (((address) + (base) - 1) & ~((base) - 1))
#define SWAP8(x) (g_swap_endian ? x : x)
#define SWAP16(x) (g_swap_endian ? __builtin_bswap16(x) : x)
#define SWAP32(x) (g_swap_endian ? __builtin_bswap32(x) : x)
#define SWAP64(x) (g_swap_endian ? __builtin_bswap64(x) : x)


typedef struct {
	const char *name;
	const u_char *mapping;
	ssize_t size;
} t_file;

// payload.asm
void payload(void);
void RC4(const u_char *data, size_t data_len, const u_char *key, size_t key_len, u_char *result);
extern uint32_t payloadSize;

// elf_common.c
void elfCommon(const t_file *fileInfo);
void elf64(const t_file *fileInfo);

// utils.c
u_char *generateEncryptionKey(void);
void displayHexData(const char *prefix, u_char *data, size_t size);