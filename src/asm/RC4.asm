[BITS 64]
DEFAULT REL

global RC4

section .bss
	S: resb 256
	T: resb 256

section .text
RC4:
	push r12					; Save preserved registers
	push r13
;	mov r11, S
;	mov r12, T
	lea r11, [S]				; Load address of S
	lea r12, [T]				; Load address of T
	xor r9, r9					; Set i to 0

init_RC4:
	push rdx
	mov rax, r9
	mov rdx, 0
	div rcx
	mov rax, rdx
	pop rdx						; rax = i % key_len

	mov r10b, byte [rdx + rax]
	mov byte [r12 + r9], r10b	; T[i] = key[i % key_len]

	mov [r11 + r9], r9			; S[i] = i

	inc r9						; If i < 256, keep looping
	cmp r9, 256
	jl init_RC4
	
	xor r9, r9					; Counter i
	xor r10, r10				; Variable j

first_loop:
	add r10b, byte [r11 + r9]	; j += S[i]
	add r10b, byte [r12 + r9]	; j += T[i]
	; j is automatically modulo-ed to 256 because of 8-bit register

	mov cl, byte [r11 + r10]
	mov dl, byte [r11 + r9]
	mov byte [r11 + r10], dl
	mov byte [r11 + r9], cl		; Swap S[i] and S[j]

	inc r9						; If i < 256, keep looping
	cmp r9, 256
	jl first_loop

	xor r9, r9					; Variable i
	xor r10, r10				; Variable j
	xor r12, r12				; Counter x

second_loop:
	inc r9b						; i = (i + 1) % 256
	add r10b, [r11 + r9]		; j = (j + S[i]) % 256
	; i and j are automatically modulo-ed to 256 because of 8-bit registers

	mov cl, byte [r11 + r10]
	mov dl, byte [r11 + r9]
	mov byte [r11 + r10], dl
	mov byte [r11 + r9], cl		; Swap S[i] and S[j]

	xor r13, r13
	mov r13b, byte [r11 + r9]
	add r13b, byte [r11 + r10]	; r13 = (S[i] + S[j]) % 256
	; r13b is automatically modulo-ed to 256 because of 8-bit registers

	xor rcx, rcx
	mov cl, byte [rdi + r12]	; cl = data[x]
	xor rdx, rdx
	mov dl, byte [r11 + r13]	; dl =  S[(S[i] + S[j]) % 256]

	xor cl, dl					; cl = data[x] ^ S[(S[i] + S[j]) % 256]
	mov byte [r8 + r12], cl		; result[x] = data[x] ^ S[(S[i] + S[j]) % 256]

	inc r12						; If x < data_len, keep looping
	cmp r12, rsi
	jl second_loop

done:
	pop r13						; Revert preserved registers
	pop r12
	ret