#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv) 
{
    int fd = open("saida.txt", O_CREAT | O_WRONLY, 0600);
    dup2(fd, 1);
    close(fd);
    printf("Hello world!\n");
    return 0;
}
