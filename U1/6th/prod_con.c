#include <linux/futex.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sys/syscall.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#define TAMANHO 10
volatile int dados[TAMANHO];
volatile size_t inserir = 0;
volatile size_t remover = 0;
_Atomic uint32_t trava = 0;

void enter_region(void){
    uint32_t v = 0;
    // ponto de linearização (onde não tem mais retorno, a função afeta algo visivel pra todo o sistema)
    if(atomic_compare_exchange_strong_explicit(&trava, &v, 1, memory_order_acquire, memory_order_relaxed)){ 
        return;
    }
    do{
        if(v==2||atomic_compare_exchange_weak_explicit(&trava, &v, 2, memory_order_relaxed, memory_order_relaxed)){
            syscall(SYS_futex, &trava, FUTEX_WAIT, 2);
        }
        v=0;
    }while(!atomic_compare_exchange_weak_explicit(&trava, &v, 2, memory_order_acquire, memory_order_relaxed)); // ponto de linearização
}

void leave_region(void){
    uint32_t v = atomic_fetch_sub_explicit(&trava, 1, memory_order_release);
    if(v != 1){
        atomic_store_explicit(&trava, 0, memory_order_relaxed);
        syscall(SYS_futex, &trava, FUTEX_WAKE, 1);
    }
}

void *produtor(void *arg){
    int v;
    for(v=1;;v++){
        enter_region();
        while((inserir+1)%TAMANHO == remover){
            leave_region();
            enter_region();
        }
        printf("Produzindo %d\n", v);
        dados[inserir] = v;
        inserir = (inserir+1)%TAMANHO;
        leave_region();
        usleep(500000);
    }
    return NULL;
}

void *consumidor(void *arg){
    for(;;){
        enter_region();
        while(inserir == remover){
            leave_region();
            enter_region(); 
        }
        printf("%zu: Consumindo %d\n",(size_t)arg,  dados[remover]);
        remover = (remover+1)%TAMANHO; 
        leave_region(); 
    }
    return NULL;
}

int main(void) {
    pthread_t th, th1, th2;
    int rc = pthread_create(&th, NULL, produtor, NULL);
    int rc1 = pthread_create(&th1, NULL, consumidor, NULL);
    int rc2 = pthread_create(&th2, NULL, consumidor, NULL);
    if (rc != 0){
        fprintf(stderr, "Erro ao criar a thread: %d\n", rc);
        exit(1);
    }
    if (rc1 != 0){
        fprintf(stderr, "Erro ao criar a thread: %d\n", rc1);
        exit(1);
    }
    pthread_join(th, NULL);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    return 0;
}

/*
1 Prod - 1 Cons
Enquanto uma CPU fica entorno de 100% a outra fica em torno de 20%

1 Prod - 2 Cons
Duas CPUs ficando entorno de 100%

Modificação do código com enter e leave region nas partes
Mas usamos 200% de CPU em média por ambos os consumidores usarem uma CPU por inteiro ao ficarem no while entrando e saindo da região gastando recursos
*/