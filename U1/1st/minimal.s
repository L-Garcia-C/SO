.text   
.global _start

_start:
    mov $0x3c, %eax
    xor %ebx, %ebx 
    syscall
