[BITS 64]
DEFAULT REL

global _start

section .data
	woody: db "....WOODY....", 10, 0
	data: dq 0x0
	dataSize: dq 0x0
	keySize: dq 0x0
	buffer: dq 0x0
	jump: dd 0x0

section .bss
	key: resb 256

section .text
%include 'RC4.asm'

_start:
print_woody:
	mov rax, 1
	mov rdi, 1
	mov rsi, woody
	mov rdx, 15
	syscall

	mov rax, 60
	xor rdi, rdi
	syscall
decrypt_text_section:
	mov rdi, data
	mov rsi, dataSize
	mov rdx, key
	mov rcx, keySize
	mov r8, buffer
	call RC4
jump_to_old_entry:
	jmp jump