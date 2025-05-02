#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

boolean novo_codigo (Codigo* c)
{
    c->byte = (U8*)calloc(1, sizeof(U8));
    if (c->byte == NULL) return FALSE;

    c->capacidade = 8;
    c->tamanho = 0;
    return TRUE;
}

void free_codigo (Codigo* c)
{
    if (c->byte != NULL) free(c->byte);
    c->byte = NULL;
    c->capacidade = 0;
    c->tamanho = 0;
}

boolean adiciona_bit (Codigo* c, U8 valor)
{

    if (c->tamanho == c->capacidade)
    {
        U8* novo = (U8*)realloc(c->byte, (c->capacidade / 8 + 1) * sizeof(U8));
        if (novo == NULL) return FALSE;
        c->byte = novo;
        c->capacidade += 8;
    }

    U8 byte_index = c->tamanho / 8;
    U8 bit_offset = c->tamanho % 8;

    if (valor == 1) {
        c->byte[byte_index] |= (1 << (7 - bit_offset));
    } else {
        c->byte[byte_index] &= ~(1 << (7 - bit_offset));
    }

    c->tamanho++;

    return TRUE;
}


boolean joga_fora_bit (Codigo* c)
{
    if (c->tamanho == 0) return FALSE;

    U8 byte_index = (c->tamanho - 1) / 8;
    U8 bit_offset = (c->tamanho - 1) % 8;

    c->byte[byte_index] &= ~(1 << (7 - bit_offset));

    c->tamanho--;

    if (c->capacidade > 8 && (c->capacidade - c->tamanho) >= 8)
    {
        U8* novo = (U8*)realloc(c->byte, (c->capacidade / 8 - 1) * sizeof(U8));
        if (novo != NULL) {
            c->byte = novo;
            c->capacidade -= 8;
        }
    }
    return TRUE;
}

boolean clone (Codigo original, Codigo* copia)
{
    copia->byte = (U8*)malloc((original.capacidade + 7) / 8 * sizeof(U8));
    if (copia->byte == NULL) return FALSE;

    memcpy(copia->byte, original.byte, (original.capacidade + 7) / 8 * sizeof(U8));

    copia->capacidade = original.capacidade;
    copia->tamanho = original.tamanho;
    return TRUE;
}