#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "types.h"

Codigo** alocar_dicionario() {
    Codigo** dicionario = (Codigo**)calloc(256, sizeof(Codigo*));
    if (!dicionario) {
        fprintf(stderr, "ERRO: Falha ao alocar dicionário\n");
        exit(EXIT_FAILURE);
    }
    return dicionario;
}

void gerar_dicionario(Codigo** dicionario_alocado, no_arvore* raiz, Codigo codigo_atual) {
    if (raiz == NULL) {
        return;
    }
    
    if(eh_folha(raiz)) {
        printf("Gerando código para byte %d (%c): \n", raiz->informacao.byte, raiz->informacao.byte);
        dicionario_alocado[raiz->informacao.byte] = (Codigo*)malloc(sizeof(Codigo));

        if(dicionario_alocado[raiz->informacao.byte] == NULL) {
            printf("Erro ao alocar memoria para Codigo no dicionario.\n");
            exit(EXIT_FAILURE);
        }

        clone(codigo_atual, dicionario_alocado[raiz->informacao.byte]);
        return;
    }

    Codigo codigo_esquerda, codigo_direita;
    clone(codigo_atual, &codigo_esquerda);
    adiciona_bit(&codigo_esquerda, 0);
    gerar_dicionario(dicionario_alocado, raiz->esquerda, codigo_esquerda);
    free_codigo(&codigo_esquerda);

    clone(codigo_atual, &codigo_direita);
    adiciona_bit(&codigo_direita, 1);
    gerar_dicionario(dicionario_alocado, raiz->direita, codigo_direita);
    free_codigo(&codigo_direita);
}

void verificar_dicionario(Codigo** dicionario) {
    if (!dicionario) return;

    printf("\n=== Verificando Dicionario ===\n");
    for (int i = 0; i < 256; i++) {
        if (dicionario[i] && dicionario[i]->tamanho > 0) {
            printf("Byte 0x%02X (%c): ", i, isprint(i) ? i : '.');
            for (int j = 0; j < dicionario[i]->tamanho; j++) {
                U8 byte_index = j / 8;
                U8 bit_offset = j % 8;
                U8 bit = (dicionario[i]->byte[byte_index] >> (7 - bit_offset)) & 1;
                printf("%d", bit);
            }
            printf("\n");
        }
    }
}

void liberar_dicionario(Codigo** dicionario) {
    if (!dicionario) return;

    for (int i = 0; i < 256; i++) {
        if (dicionario[i]) {
            free_codigo(dicionario[i]);
            dicionario[i] = NULL;    
        }
    }
    free(dicionario); 
}