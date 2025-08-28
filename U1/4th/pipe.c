#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd[2];  // fd[0] para leitura, fd[1] para escrita
    pid_t pid;

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        close(fd[0]); // Fecha o lado de leitura
        char msg[100];
        printf("Filho: Digite uma mensagem para o pai: ");
        fgets(msg, sizeof(msg), stdin);
        write(fd[1], msg, strlen(msg) + 1);  // +1 para incluir o '\0'
        close(fd[1]); // Fecha o lado de escrita após usar
    } else {
        char buffer[100];
        close(fd[1]); // Fecha o lado de escrita
        read(fd[0], buffer, sizeof(buffer)); // Escreve o conteúdo do pai no buffer
        printf("Pai recebeu: %s\n", buffer);
        close(fd[0]); // Fecha o lado de leitura após usar
    }

    return 0;
}
