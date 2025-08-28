#include <stdint.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

struct super {
    uint32_t assinatura;
    uint32_t tamanho_blocobloco;
    uint32_t nr_arquivos;
} __attribute__((packed));

struct arquivo {
    char nome[20];
    uint32_t bloco_inicial;
    uint64_t tamanho;
} __attribute__((packed));

void listar_arquivo(int argc, void *map){
    struct super *sb = (struct super *)map; 
    if (sb->assinatura != 0x5346494D){

        fprintf(stderr, "Invalid MIFS signature\n");
        exit(1);
    }

    uint32_t block_size = sb->tamanho_blocobloco;
    struct arquivo *table = (struct arquivo *)((uint8_t *)map + block_size);

    if(argc == 2){
        for (uint64_t i = 0; i < sb->nr_arquivos; i++){
            printf("%.*s\n", 20, table[i].nome);
            printf("%lu. Nome: %s, Bloco: %u, Tamanho: %lu bytes\n", i+1, table->nome, table->bloco_inicial, table->tamanho);
            table++;
        }
    }
}

void extrair_arquivo(int argc, char *argv[], void *map){
    // 1. Acessar o superbloco e a tabela de arquivos
    struct super *sb = (struct super *)map;
    uint32_t block_size = sb->tamanho_blocobloco;
    struct arquivo *file_table = (struct arquivo *)((uint8_t *)map + block_size);

    const char *file_to_extract = argv[2];
    struct arquivo *target_file_entry = NULL;
    ///*
    for (uint32_t i = 0; i < sb->nr_arquivos; i++) {
        if (strncmp(file_table[i].nome, file_to_extract, 20) == 0) {
            target_file_entry = &file_table[i];
            break;
        }
    }
    //*/
    
    //bsearch(file_to_extract, const void *base, size_t nmemb, size_t size, __compar_fn_t compar);

    if (target_file_entry == NULL) {
        fprintf(stderr, "Erro: Arquivo '%s' não encontrado na imagem.\n", file_to_extract);
        exit(1);
    }

    void *file_data_ptr = (uint8_t *)map + (target_file_entry->bloco_inicial * block_size);
    uint64_t file_size = target_file_entry->tamanho;

    int output_fd = open(file_to_extract, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd < 0) {
        perror("Erro ao criar o arquivo de saída");
        exit(1);
    }

    ssize_t bytes_written = write(output_fd, file_data_ptr, file_size);
    if (bytes_written < 0 || (uint64_t)bytes_written != file_size) {
        perror("Erro ao escrever os dados no arquivo de saída");
        close(output_fd);
        exit(1);
    }
}

int main(int argc, char *argv[]){
    
    const char *image_path = argv[1];
    int fd = open(image_path, O_RDONLY);

    if(fd < 0){
        perror("fstat");
        exit(1);
    }

    struct stat st;
    if(fstat(fd, &st) < 0){
        perror("fstat");
        exit(1);
    }

    size_t image_size = st.st_size;
    void *map = mmap(NULL, image_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(map == MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    if (argc == 2) {
        listar_arquivo(argc, map);
    } else if (argc == 3) {
        extrair_arquivo(argc, argv, map);
    }

    if(munmap(map, image_size) < 0){
        perror("munmap");
        exit(1);
    }

    close(fd);
    return 0;
}
