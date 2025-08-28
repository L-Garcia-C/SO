#include <stdio.h>
#include <unistd.h>

void g(){
    printf("fui alterado\n");
}

void f(){
    int *a[1];
    printf("Olá, ");
    a[3] = (void *) g; 
}

int main(){
    f();
    printf("Okay\n");
    return 0;
}

/*
gcc -g -o alt_flux alt_flux.c   compilar com simbolos

gdb ./alt_flux

(gdb) b f               break em f

(gdb) r                 run

(gdb) p/x &a            mostra o endereço da variavel a

(gdb) bt                mostra quem chamou o quem no código até o ponto atual

(gdb) x/10gx $rsp       mostra os 10 proximos espaços de memória a partir do registrador $rsp

            #analisar a distancia para realizar o stack overflow
*/