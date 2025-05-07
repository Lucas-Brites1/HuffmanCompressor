#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

Tabela_de_frequencias* nova_tabela_de_frequencias ()
{
    Tabela_de_frequencias* tabela = malloc(sizeof(Tabela_de_frequencias));
    if (!tabela) {
        printf("Erro de alocação de tabela\n");
        return NULL;
    }
    memset(tabela, 0, sizeof(Tabela_de_frequencias));  
    
    for (U16 i=0; i<256; i++)
        tabela->vetor[i]=NULL;

    tabela->quantidade_de_posicoes_preenchidas=0;
    printf("Nova tabela inicializada.");
    return tabela;
}

static boolean novo_no_de_arvore_binaria (Ptr_de_no_de_arvore_binaria* novo,
                                          Ptr_de_no_de_arvore_binaria esq,
                                          Ptr_de_no_de_arvore_binaria dir,
                                          elemento_t novo_elemento)
{
    *novo = (Ptr_de_no_de_arvore_binaria)malloc(sizeof(no_arvore));
    if (*novo==NULL) return FALSE;

    (*novo)->esquerda=esq;
    (*novo)->informacao=novo_elemento;
    (*novo)->direita=dir;

    return TRUE;
}

boolean inclua_byte (U8 byte, Tabela_de_frequencias* tab)
{
    if (tab->vetor[byte]!=NULL)
    {
        no_arvore* byte_atual = tab->vetor[byte];
        byte_atual->informacao.frequencia++;
        return TRUE;
    }

    elemento_t elem;
    elem.byte=byte;
    elem.frequencia=1;

    boolean OK = novo_no_de_arvore_binaria(&(tab->vetor[byte]), NULL, NULL, elem);
    if (!OK) return FALSE;

    tab->quantidade_de_posicoes_preenchidas++;
    return TRUE;
}

void junte_nodos_no_inicio_do_vetor (Tabela_de_frequencias* tab )
{
    U16 primeiro_NULL, primeiro_nao_NULL;

    for(;;) // forever
    {
        primeiro_NULL=0;
        while (primeiro_NULL<256 && tab->vetor[primeiro_NULL]!=NULL)
            primeiro_NULL++;
        if (primeiro_NULL==256) break;

        primeiro_nao_NULL=primeiro_NULL+1;
        while (primeiro_nao_NULL<256 && tab->vetor[primeiro_nao_NULL]==NULL)
            primeiro_nao_NULL++;
        if (primeiro_nao_NULL==256) break;

        tab->vetor[primeiro_NULL]=tab->vetor[primeiro_nao_NULL];
        tab->vetor[primeiro_nao_NULL]=NULL;
    }
}

