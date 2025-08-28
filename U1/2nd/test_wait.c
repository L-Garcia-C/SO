#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]) {
    pid_t completed, pid = fork();
    if(pid<0){
        return -1;
    }

    if(pid ==0){
        printf("Novo processo: %d\n", getpid());
        exit(69);
    }else{
        int status;
        printf("Processo pai: %d\n", getpid());
        completed = wait(&status);
        if (WIFEXITED(status)) { 
            status = WEXITSTATUS(status);
            printf("Processo filho: %d\n", completed);
            printf("Status: %d\n", status);
        }
    }

    return 0;
}
