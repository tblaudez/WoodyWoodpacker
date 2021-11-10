global RC4

section .text
;   void RC4(u_char *buffer, size_t bufferSize, const u_char *encryptionKey, size_t keySize)
;	RDI => buffer // RSI => bufferSize // RDX => encryptionKey // RCX = keySize
RC4:
	enter 0x200,0	; [RSP, RSP+0x100] is T{"ABC123ABC123..."} // [RSP+0x100, RSP+0x200] is S{0,1,2,3...}
	mov r9, rdx		; Copy RDX somewhere else
	xor r8, r8		; R8 is counter {0..256}

firstLoop:
	mov byte [rsp+0x100+r8], r8b	; S[R8] = R8

	mov rax, r8
	xor rdx, rdx
	div rcx			; RDX = R8 % keySize

	mov al, byte [r9+rdx]
	mov byte [rsp+r8], al	; T[i] = key[R8 % keySize]

	inc r8
	cmp r8, 0x100
	jl firstLoop		; Loop until i == 256

	xor r8, r8		; We set counters back to 0
	xor r9, r9		; RCX and RDX are not needed anymore

secondLoop:
	add r8b, byte [rsp+0x100+r9]	; R8 += S[R9]
	add r8b, byte [rsp+r9]			; R8 += T[R9] // Result is modulo-ed to 256 because of 8-bit register

	mov cl, byte [rsp+0x100+r8]		; CL = S[R8]
	mov dl, byte [rsp+0x100+r9]		; DL = S[R9]
	mov byte [rsp+0x100+r8], dl
	mov byte [rsp+0x100+r9], cl		; Swap S[R8] and S[R9]

	inc r9
	cmp r9, 0x100
	jl secondLoop					; Loop until i == 256

	xor r8, r8
	xor r9, r9
	xor r11, r11

thirdLoop:
	inc r9b							; R9 = (R9 + 1) % 256
	add r8b, [rsp+0x100+r9]			; R8 = (R8 + S[R9]) % 256 // R8 and R9 are modulo-ed to 256 because of 8-bit register

	mov cl, byte [rsp+0x100+r8]
	mov dl, byte [rsp+0x100+r9]
	mov byte [rsp+0x100+r9], cl
	mov byte [rsp+0x100+r8], dl		; Swap S[R8] and S[R9]

	mov r10b, byte [rsp+0x100+r9]
	add r10b, byte [rsp+0x100+r8]
	movsx r10, r10b					; R10 = (S[R8] + S[R9]) % 256 // R10 is modulo-ed to 256 because of 8-bit register

	mov cl, byte [rdi + r11]		; CL = buffer[R11]
	mov dl, byte [rsp+0x100+r10]	; DL = S[(S[R8] + S[R9]) % 256] // RDX is modulo-ed to 256 because of 8-bit register

	xor cl, dl						; CL = buffer[R11] ^ S[(S[R8] + S[R9]) % 256]
	mov byte [rdi + r11], cl		; buffer[r11] = buffer[R11] ^ S[(S[R8] + S[R9]) % 256]

	inc r11
	cmp r11, rsi
	jl thirdLoop					; Loop until R11 == bufferSize

done:
	leave
	ret

