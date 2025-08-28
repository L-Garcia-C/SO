#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread(void *arg) {
    printf("Ola da thread\n");
}

int main(void) {
    pthread_t th;
    int rc = pthread_create(&th, NULL, thread, NULL);
    if (rc != 0){
        fprintf(stderr, "Erro ao criar a thread: %d\n", rc);
        exit(1);
    }
    pthread_join(th, NULL);
    return 0;
}