/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // Tamanho da memória compartilhada
    size_t size = sizeof(int);

    // Cria uma região de memória compartilhada
    int *shared_mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Inicializa o valor na memória compartilhada
    *shared_mem = 10;

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {  // Processo filho
        printf("Filho: valor compartilhado = %d\n", *shared_mem);
        *shared_mem = 20;  // Alterando o valor na memória compartilhada
        printf("Filho: valor após alteração = %d\n", *shared_mem);
        exit(0);
    } else {  // Processo pai
        wait(NULL);  // Espera o filho terminar
        printf("Pai: valor compartilhado após o fork = %d\n", *shared_mem);
    }

    // Desmapeia a memória compartilhada
    if (munmap(shared_mem, size) == -1) {
        perror("munmap");
        exit(1);
    }

    return 0;
}
*/
// Funcionamento padrão
//*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // Aloca memória dinamicamente
    int *shared_mem = (int *)malloc(sizeof(int));
    
    if (shared_mem == NULL) {
        perror("malloc");
        exit(1);
    }

    // Inicializa o valor
    *shared_mem = 42;

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {  // Processo filho
        printf("Filho: valor compartilhado = %d\n", *shared_mem);
        *shared_mem = 100;  // Modificando o valor na memória
        printf("Filho: valor após alteração = %d\n", *shared_mem);
        exit(0);
    } else {  // Processo pai
        wait(NULL);  // Espera o filho terminar
        printf("Pai: valor compartilhado após o fork = %d\n", *shared_mem);
    }

    // Libera a memória alocada
    free(shared_mem);

    return 0;
}
//