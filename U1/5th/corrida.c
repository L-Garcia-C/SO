#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>

#define FALSE 0
#define TRUE  1
#define N     2

volatile int turn;
volatile int interested[N];

void enter_region(int process){
    int other = 1-process;
    interested[process]=TRUE;
    turn=process;
    atomic_thread_fence(memory_order_seq_cst); //impede a reordenação em ambos os sentidos pelo compilador e pela CPU 
    while(turn==process && interested[other]==TRUE);
    atomic_thread_fence(memory_order_seq_cst);
}

void leave_region(int process){
    atomic_thread_fence(memory_order_seq_cst);
    interested[process]=FALSE;
}

uint64_t global=0;

void *thread(void *arg) {
    int proc = (size_t)arg;
    for(int i=0; i<1000000; i++) {
        enter_region(proc);
        global++;
        leave_region(proc);
    }
    return NULL;
}

int main(void) {
    pthread_t th1, th2;
    int rc = pthread_create(&th1, NULL, thread, 0);
    int rc2 = pthread_create(&th2, NULL, thread, (void *)1);
    if (rc != 0){
        fprintf(stderr, "Erro ao criar a thread: %d\n", rc);
        exit(1);
    }
    if (rc2 != 0){
        fprintf(stderr, "Erro ao criar a thread: %d\n", rc2);
        exit(1);
    }
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    printf("Valor final: %ld\n", global);
    return 0;
}

/*
1° de 5th
#define FALSE 0
#define TRUE  1
#define N     2

int turn;
int interested[N];

void enter_region(int process){
    int other;
    other= 1-process;
    interested[process]=TRUE;
    turn=process;
    while(turn==process && interested[other]==TRUE);
}

void leave_region(int process){
    interested[process]=FALSE;
}

uint64_t global=0;

void *thread(void *arg) {
    int proc = (size_t)arg;
    for(int i=0; i<1000000; i++) {
        enter_region(proc);
        global++;
        leave_region(proc);
    }
    return NULL;
}

int main(void) {
    pthread_t th1, th2;
    int rc = pthread_create(&th1, NULL, thread, 0);
    int rc2 = pthread_create(&th2, NULL, thread, (void *)1);
    if (rc != 0){
        fprintf(stderr, "Erro ao criar a thread: %d\n", rc);
        exit(1);
    }
    if (rc2 != 0){
        fprintf(stderr, "Erro ao criar a thread: %d\n", rc2);
        exit(1);
    }
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    printf("Valor final: %ld\n", global);
    return 0;
}

Comportamento inconsistente, mas retorno próximo de 2000000

Rodando com otimização -O3 trava
Olhando o gdb vemos que fica um jmp infinito em q ambas as threads estavam pulando para a linha de instrução do próprio jmp.
Já q para o otimizador ele nunca vê a mudança do valor de interested[other] se ele for igual a True uma vez.


Ao declarar as variaveis como volateis o funcionamento volta a ser como antes sendo inconsistente e próximo de 2000000
Mas ao ativar a otimização torna-se constante, mas igual a 1000000
Isso se deve pela reordenação de instruções em que a leitura ocorre antes de entrar na região critica e atribuição depois de sair(e feita de uma vez +=1000000)

Usando a instrução " asm volatile("": : :"memory"); " logo antes do while do enter_region o assembly fica correto, mesmo com otimização, mas ainda fica inconsistente pela
concorrencia entre as duas threads. Essa instrução faz com que o compilador "desista" de reordenar para mesma região de memoria, entretanto, a execução de leitura do valor
intrested[other] pode conflitar com a instrução de intrested[process]=TRUE, fazendo com que se leia o valor FALSE antes da atribuição e ambos entram na area crítica ao
mesmo tempo, "perdendo" algumas das iterações. 

Para resolver coloca-se a função "atomic_thread_fence(memory_order_seq_cst);" em vez da " asm volatile("": : :"memory"); "
que impede a reordenação em ambos os sentidos pelo compilador e pela CPU. O que ainda permite erro pelas instruções da região
crítica poderem ser reordenadas para antes do while, mesmo que improvavel. Devendo ser posto um "atomic_thread_fence(memory_order_seq_cst);" 
após o while e antes do intrested[process]=FALSE do leave_region. Estabelecendo o funcionamento que queremos.
*/