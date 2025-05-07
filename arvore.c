#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "types.h"

no_arvore* criar_arvore(lista_t* lista) {
    if(!lista) {
        printf("Parametro lista invalido.");
        return NULL;
    }

    no_arvore *esq, *dir;
    while(lista->tamanho > 1) {
        esq = remover_primeiro_no_lista(&lista);
        dir = remover_primeiro_no_lista(&lista);

        elemento_t* informacoes_nodo_pai = (elemento_t*)malloc(sizeof(elemento_t));
        if(informacoes_nodo_pai == NULL) {
            printf("Erro de alocacao de memoria (criar arvore) \n");
            return NULL;
        }

        informacoes_nodo_pai->byte = 0; 
        informacoes_nodo_pai->frequencia = (esq->informacao.frequencia + dir->informacao.frequencia);

        no_arvore* nodo_pai_arvore =  criar_no(informacoes_nodo_pai);
        nodo_pai_arvore->esquerda = esq;
        nodo_pai_arvore->direita = dir;
        nodo_pai_arvore->proximo = NULL;

        inserir_ordenado(&lista, nodo_pai_arvore);
    }

    return lista->comeco;
}

I8 eh_folha(no_arvore* raiz) {
    return(raiz->esquerda==NULL && raiz->direita==NULL);
}

void liberar_arvore(no_arvore* raiz) {
    if (!raiz) return;
    liberar_arvore(raiz->esquerda);
    liberar_arvore(raiz->direita);
    free(raiz);
}
