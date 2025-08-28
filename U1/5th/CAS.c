#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>

uint64_t global=0;
atomic_bool trava = false;

void enter_region(void){
    bool v;
    do{
        v=false;
    }while(!atomic_compare_exchange_strong(&trava, &v, true));
}

void leave_region(void){
    atomic_store(&trava, false);
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
2° de 5th
Spin lock: Fica gastando CPU no loop while (time-real 0m0.206s)
*/