#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

uint64_t global=0; //declarar como atomico resolveria
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread(void *arg) {
    for(int i=0; i<1000000; i++) {
        pthread_mutex_lock(&mutex);
        global++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void) {
    pthread_t th1, th2;
    int rc = pthread_create(&th1, NULL, thread, NULL);
    int rc2 = pthread_create(&th2, NULL, thread, NULL);
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

/*  Originalmente com erro

uint64_t global=0; //declarar como atomico resolveria

void *thread(void *arg) {
    for(int i=0; i<1000000; i++) {
        global++;
    }
}

int main(void) {
    pthread_t th, th2;
    int rc = pthread_create(&th, NULL, thread, NULL);
    int rc2 = pthread_create(&th2, NULL, thread, NULL);
    if (rc != 0){
        fprintf(stderr, "Erro ao criar a thread: %d\n", rc);
        exit(1);
    }
    if (rc2 != 0){
        fprintf(stderr, "Erro ao criar a thread: %d\n", rc2);
        exit(1);
    }
    pthread_join(th, NULL);
    pthread_join(th2, NULL);
    printf("Valor final: %ld\n", global);
    return 0;
}


compilar com -O3 no final também resolve
*/