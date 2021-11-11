global payload:function
global payloadSize:data

payload:
%include 'RC4.asm'

section .text
printWoody:
;save stack state
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rsp
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	mov rax, 1
	mov rdi, 1
	lea rsi, [rel woody]
	mov rdx, 14
	syscall
changeProtection:
	mov rax, 10
	lea rdi, [rel payload]			; Change to .text section address
	mov rsi, 0xFFFFFFFF				; Change to .text section size
	mov rdx, 0x7					; PROT_READ | PROT_WRITE | PROT_EXEC
	syscall
decryptData:
	lea rdi, [rel payload]			; Change to .text section address
	mov rsi, 0xFFFFFFFF				; Change to .text section size
	lea rdx, [rel encryptionKey]	; Fill with encryption key
	mov	rcx, 0xFFFFFFFF				; Change to encryption key size
	call RC4

;restore stack state
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rsp
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax

	jmp 0xFFFFFFFF
payloadData:
	woody: db "....WOODY....", 10
	encryptionKey: times 0x100 db '@'
end:

section .data
    payloadSize: dd end-payload