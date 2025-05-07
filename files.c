#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

#define KB 1024
#define MB (KB*KB)

U8* ler_arquivo(const char* caminho, size_t* tamanho) {
    FILE* arquivo = fopen(caminho, "rb"); 
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir arquivo %s\n", caminho);
        return NULL;
    }

    fseek(arquivo, 0, SEEK_END);         
    *tamanho = ftell(arquivo);           
    rewind(arquivo);                     

    if (*tamanho > 200 * MB) {  
        fprintf(stderr, "Arquivo muito grande. Use processamento por blocos.\n");
        fclose(arquivo);
        return NULL;
    }

    U8* buffer = malloc(*tamanho + 1);  
    if (!buffer) {
        fclose(arquivo);
        fprintf(stderr, "Falha na alocação de %zu bytes\n", *tamanho);
        return NULL;
    }

    size_t lidos = fread(buffer, 1, *tamanho, arquivo);  
    if (lidos != *tamanho) {
        free(buffer);
        fclose(arquivo);
        fprintf(stderr, "Erro de leitura no arquivo.\n");
        return NULL;
    }

    fclose(arquivo);
    buffer[*tamanho] = '\0';  
    return buffer;
}


char* pegar_extensao_arquivo(const char* caminho) {
    I8 i = 0, j = 0;
    boolean encontrou = FALSE;    
    char* extensao = NULL;

    while(caminho[i] != '\0') {
        if(caminho[i] == '.') {
            encontrou = TRUE;
            free(extensao);
        } else if (encontrou) {
            char* temp = realloc(extensao, j+2);
            if(!temp) {
                free(extensao);
                return NULL;
            }

            extensao = temp;
            extensao[j++] = caminho[i];
            extensao[j] = '\0';
        }
        i++;
    } 

    return extensao;
}

I8 calcular_tamanho_string(const char* string) {
    I8 tamanho_string = 0;
    for(I8 i=0; string[i] != '\0'; i++) {
        tamanho_string++;
    }

    return tamanho_string;
}

char* setar_novo_nome(const char* nome_arquivo, const char* extensao_desejada) {
    I8 i = 0;
    while(nome_arquivo[i] != '\0' && nome_arquivo[i] != '.') {
        i++;
    }
    
    I8 tam_novo = i + (I8)1 + calcular_tamanho_string(extensao_desejada);
    char* novo_nome = malloc(tam_novo);
    if(!novo_nome) return NULL;
   
    I8 j=0;
    while(j!=i) {
        novo_nome[j] = nome_arquivo[j];
        j++;
    }

    novo_nome[j++]='.';
    strcpy(novo_nome + j, extensao_desejada);
    return novo_nome;
}

