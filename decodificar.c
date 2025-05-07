#include <stdio.h>
#include <stdlib.h>
#include "types.h"

Codigo* novo_codigo_decodificar() {
    Codigo* codigo = (Codigo*)malloc(sizeof(Codigo));
    if (!codigo) return NULL;
    codigo->capacidade = 1;
    codigo->tamanho = 0;
    codigo->byte = (U8*)malloc(codigo->capacidade);
    if (!codigo->byte) {
        free(codigo);
        return NULL;
    }
    return codigo;
}

void inserir_codigo_na_arvore(no_arvore** raiz, U8 byte, Codigo* codigo) {
    if (*raiz == NULL) {
        elemento_t* info = malloc(sizeof(elemento_t));
        info->byte = 0;
        info->frequencia = 0;
        *raiz = criar_no(info);
    }

    no_arvore* atual = *raiz;
    for (int i = 0; i < codigo->tamanho; i++) {
        U8 byte_index = i / 8;
        U8 bit_offset = i % 8;
        U8 bit = (codigo->byte[byte_index] >> (7 - bit_offset)) & 1;

        if (bit == 0) {
            if (!atual->esquerda) {
                elemento_t* info = malloc(sizeof(elemento_t));
                info->byte = 0;
                info->frequencia = 0;
                atual->esquerda = criar_no(info);
            }
            atual = atual->esquerda;
        } else {
            if (!atual->direita) {
                elemento_t* info = malloc(sizeof(elemento_t));
                info->byte = 0;
                info->frequencia = 0;
                atual->direita = criar_no(info);
            }
            atual = atual->direita;
        }
    }

    atual->informacao.byte = byte;
}

no_arvore* reconstruir_arvore(FILE* arquivo_codificado, U32* total_bits) {
    no_arvore* raiz = NULL;
    U8 byte_lido;
    U16 tamanho_codigo;

    for (int i = 0; i < 256; i++) {
        fread(&byte_lido, 1, 1, arquivo_codificado);
        fread(&tamanho_codigo, 2, 1, arquivo_codificado);

        if (tamanho_codigo > 0) {
            Codigo* codigo = novo_codigo_decodificar();
            codigo->capacidade = (tamanho_codigo + 7) / 8;
            codigo->tamanho = tamanho_codigo;
            free(codigo->byte);
            codigo->byte = malloc(codigo->capacidade);
            fread(codigo->byte, codigo->capacidade, 1, arquivo_codificado);
            inserir_codigo_na_arvore(&raiz, byte_lido, codigo);
            free_codigo(codigo);
        }
    }

    fread(&byte_lido, 1, 1, arquivo_codificado);
    fread(&tamanho_codigo, 2, 1, arquivo_codificado);
    if (byte_lido != 255 || tamanho_codigo != 0xFFFF) {
        fprintf(stderr, "ERRO: Marcador de fim de dicionário não encontrado.\n");
        return NULL;
    }

    fread(total_bits, sizeof(U32), 1, arquivo_codificado);

    return raiz;
}

int ler_bit(FILE* arquivo, U8* byte_atual, int* bits_restantes) {
    if (*bits_restantes == 0) {
        if (fread(byte_atual, 1, 1, arquivo) != 1) return -1;
        *bits_restantes = 8;
    }
    int bit = (*byte_atual >> 7) & 1;
    *byte_atual <<= 1;
    (*bits_restantes)--;
    return bit;
}

void decodificar(FILE* arquivo_codificado, FILE* arquivo_decodificado) {
    U32 total_bits;
    no_arvore* raiz = reconstruir_arvore(arquivo_codificado, &total_bits);
    if (!raiz) return;

    no_arvore* atual = raiz;
    U8 byte_atual = 0;
    int bits_restantes = 0;
    U32 bits_lidos = 0;


    while (bits_lidos < total_bits) {
        int bit = ler_bit(arquivo_codificado, &byte_atual, &bits_restantes);
        if (bit == -1) break;

        atual = (bit == 0) ? atual->esquerda : atual->direita;
        bits_lidos++;

        if (eh_folha(atual)) {
            fwrite(&atual->informacao.byte, 1, 1, arquivo_decodificado);
            atual = raiz;
        }
    }

    liberar_arvore(raiz);
}
