#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "types.h"

void escrever_bit_arquivo(FILE* arquivo_destino, U8 bit, U8* byte_atual, int* bits_no_byte) {
    *byte_atual = (*byte_atual << 1) | bit;
    (*bits_no_byte)++;
    if (*bits_no_byte == 8) {
        fwrite(byte_atual, 1, 1, arquivo_destino);
        *byte_atual = 0;
        *bits_no_byte = 0;
    }
}

void codificar(FILE* arquivo_entrada, FILE* arquivo_saida, Codigo** dicionario) {
    U8 byte_lido;
    U8 byte_atual = 0;
    int bits_no_byte = 0;
    U32 total_bits_escritos = 0;

    printf("[DEBUG-CODIFICAR] Iniciando escrita do dicionário.\n");

    for (int i = 0; i < 256; i++) {
        fwrite(&i, 1, 1, arquivo_saida); 
        if (dicionario[i] && dicionario[i]->tamanho > 0) {
            U16 tamanho = (U16)dicionario[i]->tamanho;
            fwrite(&tamanho, 2, 1, arquivo_saida);
            fwrite(dicionario[i]->byte, (tamanho + 7) / 8, 1, arquivo_saida);
            printf("[DEBUG-CODIFICAR] Escrevendo código para byte 0x%02X (%c), tamanho: %u\n",
                   i, isprint(i) ? i : '.', tamanho);
        } else {
            U16 tamanho_zero = 0;
            fwrite(&tamanho_zero, 2, 1, arquivo_saida);
            printf("[DEBUG-CODIFICAR] Byte 0x%02X não tem código (tamanho 0).\n", i);
        }
    }

    U8 fim_dicionario = 255;
    U16 tamanho_fim = 0xFFFF;
    fwrite(&fim_dicionario, 1, 1, arquivo_saida);
    fwrite(&tamanho_fim, 2, 1, arquivo_saida);
    printf("[DEBUG-CODIFICAR] Marcador de fim do dicionário escrito.\n");

    long pos_total_bits = ftell(arquivo_saida);
    fwrite(&total_bits_escritos, sizeof(U32), 1, arquivo_saida); 

    printf("[DEBUG-CODIFICAR] Iniciando escrita dos dados codificados.\n");

    while (fread(&byte_lido, 1, 1, arquivo_entrada) == 1) {
        Codigo* codigo = dicionario[byte_lido];
        if (codigo && codigo->tamanho > 0) {
            for (int i = 0; i < codigo->tamanho; i++) {
                U8 byte_index = i / 8;
                U8 bit_offset = i % 8;
                U8 bit = (codigo->byte[byte_index] >> (7 - bit_offset)) & 1;
                escrever_bit_arquivo(arquivo_saida, bit, &byte_atual, &bits_no_byte);
                total_bits_escritos++;
            }
        } else {
            fprintf(stderr, "ERRO: Byte não encontrado no dicionário: %d\n", byte_lido);
            exit(EXIT_FAILURE);
        }
    }

    if (bits_no_byte > 0) {
        byte_atual <<= (8 - bits_no_byte);
        fwrite(&byte_atual, 1, 1, arquivo_saida);
        total_bits_escritos += bits_no_byte;
    }

    long pos_fim = ftell(arquivo_saida);
    fseek(arquivo_saida, pos_total_bits, SEEK_SET);
    fwrite(&total_bits_escritos, sizeof(U32), 1, arquivo_saida);
    fseek(arquivo_saida, pos_fim, SEEK_SET);

    printf("[DEBUG-CODIFICAR] Total de bits escritos: %u\n", total_bits_escritos);
    printf("[DEBUG-CODIFICAR] Escrita dos dados codificados concluída.\n");
}