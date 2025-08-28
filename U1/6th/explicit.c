#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>

uint64_t global = 0;
_Atomic uint32_t trava = 0;

void enter_region(void){
    uint32_t v = 0;
    // ponto de linearização (onde não tem mais retorno, a função afeta algo visivel pra todo o sistema)
    if(atomic_compare_exchange_strong_explicit(&trava, &v, 1, memory_order_acquire, memory_order_relaxed)){ 
        return;
    }
    do{
        if(v!=0&&(v==2||atomic_compare_exchange_weak_explicit(&trava, &v, 2, memory_order_relaxed, memory_order_relaxed))){
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

void *thread(void *arg) {
    for(int i=0; i<1000000; i++) {
        enter_region();
        global++;
        leave_region();
    }
    return NULL;
}

int main(void) {
    pthread_t th1, th2;
    int rc = pthread_create(&th1, NULL, thread, NULL);
    int rc2 = pthread_create(&th2, NULL, thread, NULL);
    
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    printf("Valor final: %ld\n", global);
    return 0;
}

/*
Atividade para ver eficiancia no tipo de barreira, relaxar as desnecessariamente fortes.
memory order acquire - Necessaria em pontos de linerialização (quando secesso e mudando os valores) ~impede as coisas de subir
memory order relaxed - Intermediaria
memory order release - Mal é necessaria uma barreira                                                ~impede as coisas de descer
Utilização o CAS weak para ganho de performace em loop

*/