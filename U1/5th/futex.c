#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>

uint64_t global=0;
_Atomic uint32_t trava = false;

void enter_region(void){
    uint32_t v;
    do{
        syscall(SYS_futex, &trava, FUTEX_WAIT, 1);
        v=atomic_exchange(&trava, 1);
    }while(v);
}

void leave_region(void){
    atomic_store(&trava, 0);
    syscall(SYS_futex, &trava, FUTEX_WAKE, 1);
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
3° de 5th
Fica muito lento, pois faz muitas syscalls(time-real 0m2.233s)
*/