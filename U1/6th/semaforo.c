//Implementação com Mutex da anterior e implementação de semaforos para melhorar a eficiencia
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define TAMANHO 10
volatile int dados[TAMANHO];
volatile size_t inserir = 0;
volatile size_t remover = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct semaforo sem_fila;
void sem_inicializar(struct semaforo *s);


struct esperando{
    pthread_mutex_t m;
    struct esperando *prox;
};

struct semaforo{
    pthread_mutex_t trava;
    size_t valor;
    struct esperando *cabeca;
    struct esperando *cauda;
};

void sem_inicializar(struct semaforo *s){
    pthread_mutex_init(&s->trava, NULL);
    s->valor = 0;
    s->cabeca = NULL;
    s->cauda = NULL;
    return;
}


void sem_incrementar(struct semaforo *s){
    struct esperando *esp;
    pthread_mutex_lock(&s->trava);
    esp = s->cabeca;
    if(esp != NULL){
        s->cabeca = esp->prox;
        if(!s->cabeca){
            s->cauda = NULL;
        }
    }
    s->valor++;
    pthread_mutex_unlock(&s->trava);
    if(esp){
        pthread_mutex_unlock(&esp->m);
    }
    return;
}

void sem_decrementar(struct semaforo *s){
    for(;;){
        pthread_mutex_lock(&s->trava);
        if(s->valor>0){
            s->valor--;
            pthread_mutex_unlock(&s->trava);
            return;
        }
        
        struct esperando *novo = malloc(sizeof(struct esperando));
        pthread_mutex_init(&novo->m, NULL);
        pthread_mutex_lock(&novo->m);
        novo->prox = NULL;
        if(s->cauda!=NULL){
            s->cauda->prox = novo;
        }else{
           s->cabeca = novo;
        }
        s->cauda = novo;
        pthread_mutex_unlock(&s->trava);
        pthread_mutex_lock(&novo->m);
    }
}


void *produtor(void *arg){
    int v;
    for(v=1;;v++){
        pthread_mutex_lock(&mutex);
        while((inserir+1)%TAMANHO == remover){
            pthread_mutex_unlock(&mutex);
            pthread_mutex_lock(&mutex);
        }
        printf("Produzindo %d\n", v);
        dados[inserir] = v;
        inserir = (inserir+1)%TAMANHO;
        pthread_mutex_unlock(&mutex);
        sem_incrementar(&sem_fila);
        usleep(500000);
    }
    return NULL;
}

void *consumidor(void *arg){
    for(;;){
        sem_decrementar(&sem_fila);
        pthread_mutex_lock(&mutex);
        printf("%zu: Consumindo %d\n",(size_t)arg,  dados[remover]);
        remover = (remover+1)%TAMANHO; 
        pthread_mutex_unlock(&mutex); 
    }
    return NULL;
}

int main(void) {
    pthread_t th, th1, th2;
    sem_inicializar(&sem_fila);
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
NÃO ESTÁ FUNCIONANDO
Se muitos produtores, ocorre mesmo problema de quando muitos consumidores(ficar etrando e saindo da região crítica no while)
*/