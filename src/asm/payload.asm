global payload:function
global payloadSize:data

payload:
%include 'RC4.asm'

section .text
printWoody:
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

	jmp 0xFFFFFFFF
payloadData:
	woody: db "....WOODY....", 10
	encryptionKey: times 0x100 db '@'
end:

section .data
    payloadSize: dd end-payload