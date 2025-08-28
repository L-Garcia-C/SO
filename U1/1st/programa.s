.data
    msg:    .asciz "Hello, World!" 

.text   
.global _start

_start:

    mov $1, %eax        
    mov $1, %rdi        
    lea msg(%rip), %rsi 
    mov $0xd, %rdx
    syscall

    mov $0x3c, %eax
    xor %edi, %edi
    syscall
