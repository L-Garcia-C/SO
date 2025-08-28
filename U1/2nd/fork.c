#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Erro no fork");
        return 1;
    }else if (pid == 0) {
        printf("Eu sou o filho e meu PID é %d\n", getpid());
    } else {
        printf("Eu sou o Pai e meu PID é %d\n",getpid());
    }
    return 0;
}
