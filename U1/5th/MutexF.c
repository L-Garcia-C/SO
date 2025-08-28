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
    if(atomic_compare_exchange_strong(&trava, &v, 1)){
        return;
    }
    do{
        if(v==2||atomic_compare_exchange_strong(&trava, &v, 2)){
            syscall(SYS_futex, &trava, FUTEX_WAIT, 2);
        }
        v=0;
    }while(!atomic_compare_exchange_strong(&trava, &v, 2));
}

void leave_region(void){
    uint32_t v = atomic_fetch_sub(&trava, 1);
    if(v != 1){
        atomic_store(&trava, 0);
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
4° de 5th
Solução do problema da trava usando futex para implementar Mutex muiro mais rapida e menos custosa(time-real  0m0.105s)
*/